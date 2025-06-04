//
// Created by zeng on 24-6-25.
//

#include "BiliLiveSession.h"

#include "../../Entity/Global/Config.h"
#include "../../Entity/Global/Logger.h"
#include "../../Entity/ThreadPool/ProcessingMessageThreadPool.h"
#include "BiliApiUtil.h"
#include "BiliRequestHeader.h"

#include <boost/beast/http.hpp>
#include <nlohmann/json.hpp>
#include <regex>
#include <thread>

BiliLiveSession::BiliLiveSession(boost::asio::io_context& ioc)
    : ioc(ioc)
    , ctx{boost::asio::ssl::context::tlsv12_client}
    , resolver{boost::asio::make_strand(ioc)}
    , ws{boost::asio::make_strand(ioc), ctx}
    , host("")
    , target("/sub")
    , pingTimer{ioc}
    , stopFlag{true}
{
}

bool BiliLiveSession::InitSSLCert()
{
    // // 判断证书是否存在
    // if (!std::filesystem::exists("/etc/ssl/certs/ca-certificates.crt"))
    // {
    //     LOG_MESSAGE(LogLevel::Error, "Failed to find ssl cert");
    //     return false;
    // }
    // // 加载证书
    // this->ctx.load_verify_file("/etc/ssl/certs/ca-certificates.crt");
    // // 设置证书验证模式
    // this->ctx.set_verify_mode(boost::asio::ssl::verify_peer);
    return true;
}

bool BiliLiveSession::InitRoomInfo()
{
    LOG_VAR(LogLevel::Debug, Config::GetInstance()->GetDanmuSeverConfUrl().GetHost());
    LOG_VAR(LogLevel::Debug, Config::GetInstance()->GetDanmuSeverConfUrl().GetPort());
    LOG_VAR(LogLevel::Debug, Config::GetInstance()->GetDanmuSeverConfUrl().GetTarget());
    LOG_VAR(LogLevel::Debug,
            Config::GetInstance()->GetDanmuSeverConfUrl().GetTargetWithWbiParamSafeQuery());
    // 解析域名和端口
    const auto results = this->resolver.resolve(
        Config::GetInstance()->GetDanmuSeverConfUrl().GetHost(),
        std::to_string(Config::GetInstance()->GetDanmuSeverConfUrl().GetPort()));

    // 创建SSL流并连接
    boost::asio::io_context                                iocHttp;
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket> stream(iocHttp, this->ctx);
    boost::asio::connect(stream.next_layer(), results.begin(), results.end());
    stream.handshake(boost::asio::ssl::stream_base::client);
    // 构建GET请求
    boost::beast::http::request<boost::beast::http::string_body> req{
        boost::beast::http::verb::get,
        Config::GetInstance()->GetDanmuSeverConfUrl().GetTargetWithWbiParamSafeQuery(),
        11};
    req.set(boost::beast::http::field::host,
            Config::GetInstance()->GetDanmuSeverConfUrl().GetHost());
    req.set(boost::beast::http::field::user_agent,
            BiliRequestHeader::GetInstance()->GetUserAgent());
    // 设置cookie
    req.set(boost::beast::http::field::cookie,
            BiliRequestHeader::GetInstance()->GetBiliCookie().ToString());
    // 发送请求
    boost::beast::http::write(stream, req);

    // 接收响应
    boost::beast::flat_buffer                                      buffer;
    boost::beast::http::response<boost::beast::http::dynamic_body> res;
    boost::beast::http::read(stream, buffer, res);
    if (res.result() != boost::beast::http::status::ok)
    {
        LOG_VAR(LogLevel::Error, res.result_int());
        return false;
    }
    // 解析json
    std::string    resStr        = boost::beast::buffers_to_string(res.body().data());
    nlohmann::json danmuInfoJson = nlohmann::json::parse(resStr);
    LOG_VAR(LogLevel::Debug, danmuInfoJson.dump(-1));
    if (danmuInfoJson["code"] != 0)
    {
        LOG_VAR(LogLevel::Error, danmuInfoJson["message"].dump(4));
        return false;
    }
    for (const auto& item : danmuInfoJson["data"]["host_list"])
    {
        this->liveUrls.emplace_back(item["host"].get<std::string>(),
                                    item["port"].get<unsigned>(),
                                    item["ws_port"].get<unsigned>(),
                                    item["wss_port"].get<unsigned>());
    }
    this->token = danmuInfoJson["data"]["token"].get<std::string>();
    return true;
}

bool BiliLiveSession::run()
{
    if (!this->stopFlag.load())
    {
        LOG_MESSAGE(LogLevel::Error, "Thread pool is already running");
        return false;
    }

    this->stopFlag.store(false);
    if (!this->InitSSLCert())
    {
        LOG_MESSAGE(LogLevel::Error, "Failed to init ssl cert");
        this->stop();
        return false;
    }
    if (!this->InitRoomInfo())
    {
        LOG_MESSAGE(LogLevel::Error, "Failed to init room info");
        this->stop();
        return false;
    }
    // 异步解析域名
    this->resolver.async_resolve(
        this->liveUrls.back().host,
        std::to_string(this->liveUrls.back().wss_port),
        boost::beast::bind_front_handler(&BiliLiveSession::on_resolve, shared_from_this()));
    return true;
    // this->ws.async_read(
    //     this->buffer,
    //     boost::beast::bind_front_handler(&BiliLiveSession::on_read, shared_from_this()));
}

void BiliLiveSession::stop()
{
    // 原子地设置 stopFlag 为 true，并获取之前的值
    // 如果之前已经是 true，说明停止操作已在进行或已完成
    if (stopFlag.exchange(true, std::memory_order_acq_rel))
    {
        LOG_MESSAGE(LogLevel::Info,
                    "BiliLiveSession stop procedure already initiated or completed.");
        return;
    }

    LOG_MESSAGE(LogLevel::Info, "Stopping BiliLiveSession...");

    // 使用 post 将取消/关闭操作提交到 io_context，以确保线程安全
    // 并且这些操作在 io_context 的事件循环中执行

    // 1. 取消 pingTimer
    boost::asio::post(ioc, [self = shared_from_this()]() {
        self->pingTimer.cancel();
        LOG_MESSAGE(LogLevel::Debug, "Ping timer cancelled successfully.");
    });

    // 2. 取消 resolver (如果仍在解析)
    // resolver 的操作通常在自己的 strand 上，但取消本身是线程安全的。
    // 为了统一，也可以 post 到 ioc。或者直接在 resolver 的 strand 上操作。
    // 由于 resolver 是在 ioc 上创建的 make_strand(ioc)，post到ioc是安全的。
    boost::asio::post(ioc, [self = shared_from_this()]() {
        self->resolver.cancel();   // Resolver的cancel是线程安全的
        LOG_MESSAGE(LogLevel::Debug, "Resolver operations cancelled.");
    });


    // 3. 关闭 WebSocket stream
    // 这将导致任何挂起的 async_read 或 async_write 操作以错误完成
    boost::asio::post(
        ws.get_executor(), [self = shared_from_this()]() {   // 在 websocket 的 strand 上执行
            if (self->ws.is_open())
            {
                // 直接关闭底层TCP套接字，这会使所有WebSocket操作失败
                boost::beast::get_lowest_layer(self->ws).close();
                LOG_MESSAGE(LogLevel::Debug, "WebSocket lowest layer closed successfully.");
                // 可选：尝试发送一个 WebSocket close 帧，但这可能不会成功，如果底层已关闭
                // self->ws.async_close(boost::beast::websocket::close_code::normal,
                //    boost::beast::bind_front_handler(&BiliLiveSession::on_ws_closed_logging,
                //    self->shared_from_this()));
            }
            else
            {
                LOG_MESSAGE(LogLevel::Debug, "WebSocket was not open, no need to close.");
            }
        });
    LOG_MESSAGE(LogLevel::Info, "BiliLiveSession stop sequence posted to IO context.");
}

void BiliLiveSession::on_resolve(boost::beast::error_code                            ec,
                                 const boost::asio::ip::tcp::resolver::results_type& results)
{
    // 如果有错误则返回错误信息
    if (ec)
    {
        LOG_VAR(LogLevel::Error, ec.message());
        return;
    }
    if (this->stopFlag.load())
    {
        LOG_MESSAGE(LogLevel::Error, "BiliLiveSession is stopped");
        return;
    }
    // 设置超时时间
    boost::beast::get_lowest_layer(this->ws).expires_after(std::chrono::seconds(30));

    // 异步连接
    boost::beast::get_lowest_layer(this->ws).async_connect(
        results,
        boost::beast::bind_front_handler(&BiliLiveSession::on_connect, shared_from_this()));
}

void BiliLiveSession::on_connect(boost::beast::error_code                                    ec,
                                 boost::asio::ip::tcp::resolver::results_type::endpoint_type ep)
{
    if (ec)
    {
        LOG_VAR(LogLevel::Error, ec.message());
        return;
    }
    if (this->stopFlag.load())
    {
        LOG_MESSAGE(LogLevel::Error, "BiliLiveSession is stopped");
        return;
    }
    // Set a timeout on the operation
    boost::beast::get_lowest_layer(this->ws).expires_after(std::chrono::seconds(30));

    // Set SNI Hostname (many hosts need this to handshake successfully)
    if (!SSL_set_tlsext_host_name(this->ws.next_layer().native_handle(),
                                  this->liveUrls.back().host.c_str()))
    {
        ec = boost::beast::error_code(static_cast<int>(::ERR_get_error()),
                                      boost::asio::error::get_ssl_category());
        LOG_VAR(LogLevel::Error, ec.message());
        return;
    }

    // Update the host_ string. This will provide the value of the
    // Host HTTP header during the WebSocket handshake.
    // See https://tools.ietf.org/html/rfc7230#section-5.4
    this->host = fmt::format("{}:{}", this->liveUrls.back().host, ep.port());

    // Perform the SSL handshake
    this->ws.next_layer().async_handshake(
        boost::asio::ssl::stream_base::client,
        boost::beast::bind_front_handler(&BiliLiveSession::on_ssl_handshake, shared_from_this()));
}

void BiliLiveSession::on_ssl_handshake(boost::beast::error_code ec)
{
    if (ec)
    {
        LOG_VAR(LogLevel::Error, ec.message());
        return;
    }
    if (this->stopFlag.load())
    {
        LOG_MESSAGE(LogLevel::Error, "BiliLiveSession is stopped");
        return;
    }
    // Turn off the timeout on the tcp_stream, because
    // the websocket stream has its own timeout system.
    boost::beast::get_lowest_layer(this->ws).expires_never();

    // Set suggested timeout settings for the websocket
    this->ws.set_option(
        boost::beast::websocket::stream_base::timeout::suggested(boost::beast::role_type::client));

    // Set a decorator to change the User-Agent of the handshake
    this->ws.set_option(boost::beast::websocket::stream_base::decorator(
        [](boost::beast::websocket::request_type& req) {
            req.set(boost::beast::http::field::user_agent,
                    BiliRequestHeader::GetInstance()->GetUserAgent());
        }));

    this->ws.set_option(boost::beast::websocket::stream_base::decorator(
        [](boost::beast::websocket::request_type& req) {
            req.set(boost::beast::http::field::cookie,
                    BiliRequestHeader::GetInstance()->GetBiliCookie().ToString());
        }));
    // Perform the websocket handshake
    this->ws.async_handshake(
        this->host,
        this->target,
        boost::beast::bind_front_handler(&BiliLiveSession::on_handshake, shared_from_this()));
}

void BiliLiveSession::on_handshake(boost::beast::error_code ec)
{
    if (ec)
    {
        LOG_VAR(LogLevel::Error, ec.message());
        return;
    }
    if (this->stopFlag.load())
    {
        LOG_MESSAGE(LogLevel::Error, "BiliLiveSession is stopped");
        return;
    }
    nlohmann::json body;
    body = {
        {"roomid", Config::GetInstance()->GetRoomId()},
        {"uid", BiliRequestHeader::GetInstance()->GetBiliCookie().GetDedeUserID()},
        // {"uid", 0},
        {"protover", 3},
        {"platform", "web"},
        {"type", 2},
        {"buvid", BiliRequestHeader::GetInstance()->GetBiliCookie().GetBuvid3()},
        {"key", this->token},
    };
    // LOG_VAR(LogLevel::Debug, body.dump(4));
    // LOG_VAR(LogLevel::Debug, body.dump(-1));
    // std::string bodyStr = R"({"roomid": )" + std::to_string(Config::GetInstance()->GetRoomId()) +
    //                       R"(, "key": ")" + this->token + R"("})";
    // std::string bodyStr =
    //     "{" +
    //     fmt::format(
    //         R"("uid": {}, "roomid": {}, "protover": 3, "platform": "web", "type": 2, "key": "{}",
    //         "buvid": {})",
    //         // BiliRequestHeader::GetInstance()->GetBiliCookie().GetDedeUserID(),
    //         0,
    //         Config::GetInstance()->GetRoomId(),
    //         this->token,
    //         BiliRequestHeader::GetInstance()->GetBiliCookie().GetBuvid3()) +
    //     "}";
    // LOG_VAR(LogLevel::Debug, bodyStr);
    std::string          bodyStr = body.dump(-1);
    std::vector<uint8_t> authMessage;
    BiliApiUtil::MakePack(body.dump(-1), BiliApiUtil::Operation::AUTH, authMessage);
    // Send the message
    this->ws.async_write(
        boost::asio::buffer(authMessage),
        boost::beast::bind_front_handler(&BiliLiveSession::on_auth, shared_from_this()));
}

void BiliLiveSession::on_write(boost::beast::error_code ec, std::size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred);

    if (ec)
    {
        LOG_VAR(LogLevel::Error, ec.message());
        return;
    }
    if (this->stopFlag.load())
    {
        LOG_MESSAGE(LogLevel::Warn, "BiliLiveSession is stopped");
        return;
    }
    if (this->stopFlag.load())
    {
        LOG_MESSAGE(LogLevel::Error, "BiliLiveSession is stopped");
        return;
    }
    // // std::this_thread::sleep_for(std::chrono::seconds(5));
    // // Read a message into our buffer
    // this->ws.async_read(
    //     this->buffer,
    //     boost::beast::bind_front_handler(&BiliLiveSession::on_read, shared_from_this()));
}

void BiliLiveSession::on_auth(boost::beast::error_code ec, std::size_t bytes_transferred)
{
    if (ec)
    {
        LOG_VAR(LogLevel::Error, ec.message());
        return;
    }
    if (this->stopFlag.load())
    {
        LOG_MESSAGE(LogLevel::Warn, "BiliLiveSession is stopped");
        return;
    }
    this->ws.async_read(
        this->buffer,
        boost::beast::bind_front_handler(&BiliLiveSession::on_read, shared_from_this()));
    // 定时发送心跳包
    this->pingTimer.expires_after(std::chrono::seconds(28));
    this->pingTimer.async_wait(
        [this](const boost::system::error_code& ec) { this->ping_task(ec); });

    // this->pingtimer =
    //     std::move(this->runtime.timer_queue()->make_timer(std::chrono::seconds(0),
    //                                                       std::chrono::seconds(28),
    //                                                       this->runtime.thread_pool_executor(),
    //                                                       [this] { this->do_ping(); }));
    // this->pingThread = std::move(std::jthread(&BiliLiveSession::do_ping, this));
    // this->pingThread.detach();
}

void BiliLiveSession::on_read(boost::beast::error_code ec, std::size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred);
    if (ec)
    {
        LOG_VAR(LogLevel::Error, ec.message());
        return;
    }
    if (this->stopFlag.load())
    {
        LOG_MESSAGE(LogLevel::Warn, "BiliLiveSession is stopped");
        return;
    }
    // std::cout << boost::beast::make_printable(this->buffer.data()) << std::endl;
    // LOG_VAR(LogLevel::Debug, std::string(boost::beast::buffers_to_string(this->buffer.data())));
    // 将this->buffer.data()转为std::vector<uint8_t>
    auto                 buf     = this->buffer.data();
    auto                 bufData = buf.data();
    std::vector<uint8_t> response;
    response.assign(static_cast<const uint8_t*>(bufData),
                    static_cast<const uint8_t*>(bufData) + boost::asio::buffer_size(buf));
    this->buffer.consume(response.size());
    auto pack = BiliApiUtil::Unpack(response);
    for (auto& item : pack)
    {
        // LOG_VAR(LogLevel::Info, std::get<0>(item).ToString());
        // LOG_VAR(LogLevel::Debug, item);
        // std::regex r(R"(\\)");                                  // 正则表达式匹配'\'
        // std::string result = std::regex_replace(item, r, "");   // 用空字符串替换所有匹配的'\'
        // try
        // {
        //     nlohmann::json json = nlohmann::json::parse(content);
        //     LOG_VAR(LogLevel::Debug, json.dump(-1));
        // }
        // catch (const nlohmann::json::exception& e)
        // {
        //     LOG_VAR(LogLevel::Error, e.what());
        //     LOG_VAR(LogLevel::Error, content);
        //     // LOG_VAR(LogLevel::Error, result);
        // }
        LOG_VAR(LogLevel::Debug, std::get<1>(item));
        ProcessingMessageThreadPool::GetInstance()->AddTask(std::move(item));
    }
    this->ws.async_read(
        this->buffer,
        boost::beast::bind_front_handler(&BiliLiveSession::on_read, shared_from_this()));
}

void BiliLiveSession::on_close(boost::beast::error_code ec)
{
    if (ec)
    {
        LOG_VAR(LogLevel::Error, ec.message());
        return;
    }
    // If we get here then the connection is closed gracefully

    // The make_printable() function helps print a ConstBufferSequence
    // std::cout << boost::beast::make_printable(this->buffer.data()) << std::endl;
    // this->pingThread.request_stop();   // 停止心跳线程
    // this->runtime.shutdown();
}

void BiliLiveSession::do_ping()
{
    // for (;;)
    // {
    //     std::vector<uint8_t> authMessage;
    //     BiliApiUtil::MakePack("{}", BiliApiUtil::Operation::HEARTBEAT, authMessage);
    //     // Send the message
    //     this->ws.async_write(
    //         boost::asio::buffer(authMessage),
    //         boost::beast::bind_front_handler(&BiliLiveSession::on_write, shared_from_this()));
    //     // this->ws.write(boost::asio::buffer(authMessage));
    //     std::this_thread::sleep_for(std::chrono::seconds(29));
    // }
    if (this->stopFlag.load())
    {
        LOG_MESSAGE(LogLevel::Error, "BiliLiveSession is stopped");
        return;
    }

    std::vector<uint8_t> authMessage;
    BiliApiUtil::MakePack("{}", BiliApiUtil::Operation::HEARTBEAT, authMessage);
    // Send the message
    LOG_MESSAGE(LogLevel::Debug, "do_ping");
    this->ws.async_write(
        boost::asio::buffer(authMessage),
        boost::beast::bind_front_handler(&BiliLiveSession::on_write, shared_from_this()));
}
void BiliLiveSession::ping_task(const boost::system::error_code& ec)
{
    if (ec)
    {
        LOG_VAR(LogLevel::Error, ec.message());
        return;
    }
    if (this->stopFlag.load())
    {
        LOG_MESSAGE(LogLevel::Error, "BiliLiveSession is stopped");
        return;
    }
    this->do_ping();
    // 重新设置定时器
    this->pingTimer.expires_after(std::chrono::seconds(28));
    this->pingTimer.async_wait([this](const boost::system::error_code& ec) {
        if (this->stopFlag.load())
        {
            return;   // 如果停止了，就不再执行心跳任务
        }
        this->ping_task(ec);
    });
}