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

BiliLiveSession::BiliLiveSession()
    : ctx{boost::asio::ssl::context::tlsv12_client}
    , resolver{boost::asio::make_strand(ioc)}
    , ws{boost::asio::make_strand(ioc), ctx}
    , host("")
    , target("/sub")
    , pingTimer{ioc}
    , stopFlag{true}   // Initialize stopFlag to true (stopped state)
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
        this->liveUrls.emplace_back(
            item["host"].get<std::string>(),
            item["port"].get<unsigned>(),
            item["port"].get<unsigned>(),   // Assuming ws_port and wss_port are distinct, ensure
                                            // this is correct
            item["wss_port"].get<unsigned>());
    }
    this->token = danmuInfoJson["data"]["token"].get<std::string>();
    return true;
}

bool BiliLiveSession::run()
{
    if (!this->stopFlag.load())
    {
        LOG_MESSAGE(LogLevel::Error, "BiliLiveSession is already running");
        return false;
    }

    this->stopFlag.store(false);   // Set stopFlag to false (running state)
    if (!this->InitSSLCert())
    {
        LOG_MESSAGE(LogLevel::Error, "Failed to init ssl cert");
        this->stop();   // Call stop to clean up
        return false;
    }
    if (!this->InitRoomInfo())
    {
        LOG_MESSAGE(LogLevel::Error, "Failed to init room info");
        this->stop();   // Call stop to clean up
        return false;
    }
    // 异步解析域名
    this->resolver.async_resolve(
        this->liveUrls.back().host,
        std::to_string(this->liveUrls.back().wss_port),
        boost::beast::bind_front_handler(&BiliLiveSession::on_resolve, shared_from_this()));
    return this->ioc.run();
}

void BiliLiveSession::stop()
{
    LOG_MESSAGE(LogLevel::Info, "BiliLiveSession::stop() called.");
    // 1. 设置 stopFlag 为 true，立即通知所有进行中的异步操作停止。
    // this->stopFlag.store(true);
    if (this->stopFlag.exchange(true))
    {
        LOG_MESSAGE(LogLevel::Warn, "stop 已执行，跳过");
        return;
    }

    // 2. 取消 pingTimer。
    // 这将导致任何挂起的 pingTimer::async_wait 操作以 boost::asio::error::operation_aborted 完成。
    // 根据编译错误，pingTimer.cancel() 不接受 error_code 参数。
    this->pingTimer.cancel();
    LOG_MESSAGE(LogLevel::Debug, "pingTimer cancellation requested.");

    // 3. 优雅地关闭 WebSocket 连接。
    // 这将发送一个 WebSocket 关闭帧，并取消 ws 上任何挂起的 async_read 或 async_write 操作。
    // 它们的处理程序将以 boost::asio::error::operation_aborted 调用。
    if (this->ws.is_open())
    {
        this->ws.async_close(
            boost::beast::websocket::close_reason(boost::beast::websocket::close_code::normal),
            boost::beast::bind_front_handler(&BiliLiveSession::on_close, shared_from_this()));
        LOG_MESSAGE(LogLevel::Debug, "WebSocket async_close initiated.");
    }
    else
    {
        LOG_MESSAGE(LogLevel::Warn, "WebSocket is not open, no need to async_close.");
    }

    // 4. 取消域名解析器。
    // 根据编译错误，resolver.cancel() 不接受 error_code 参数。
    this->resolver.cancel();
    LOG_MESSAGE(LogLevel::Debug, "Resolver cancellation requested.");
    this->ioc.stop();
    LOG_MESSAGE(LogLevel::Info, "BiliLiveSession::stop() finished initiating shutdown.");
}

// ... (BiliLiveSession 剩余的回调函数，它们内部的 stopFlag 检查和错误处理保持不变)

void BiliLiveSession::on_resolve(boost::beast::error_code                            ec,
                                 const boost::asio::ip::tcp::resolver::results_type& results)
{
    // 如果有错误或者会话已停止，则返回。
    if (ec)
    {
        LOG_VAR(LogLevel::Error, ec.message());
        return;
    }
    if (this->stopFlag.load())
    {
        LOG_MESSAGE(LogLevel::Warn, "BiliLiveSession is stopped, on_resolve returning.");
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
        LOG_MESSAGE(LogLevel::Warn, "BiliLiveSession is stopped, on_connect returning.");
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
        LOG_MESSAGE(LogLevel::Warn, "BiliLiveSession is stopped, on_ssl_handshake returning.");
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
        LOG_MESSAGE(LogLevel::Warn, "BiliLiveSession is stopped, on_handshake returning.");
        return;
    }
    nlohmann::json body;
    body = {
        {"roomid", Config::GetInstance()->GetRoomId()},
        {"uid", BiliRequestHeader::GetInstance()->GetBiliCookie().GetDedeUserID()},
        {"protover", 3},
        {"platform", "web"},
        {"type", 2},
        {"buvid", BiliRequestHeader::GetInstance()->GetBiliCookie().GetBuvid3()},
        {"key", this->token},
    };
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
        // 如果错误是 operation_aborted，意味着写入被取消，这在关闭期间是预期的。
        if (ec == boost::asio::error::operation_aborted)
        {
            LOG_MESSAGE(LogLevel::Info,
                        "BiliLiveSession on_write: Operation aborted (expected during shutdown).");
        }
        else if (ec == boost::beast::websocket::error::closed)
        {
            LOG_MESSAGE(LogLevel::Info,
                        "BiliLiveSession on_write: WebSocket closed (expected during shutdown).");
        }
        else
        {
            LOG_VAR(LogLevel::Error, ec.message());
        }
        this->stopFlag.store(true);
        return;
    }
    if (this->stopFlag.load())
    {
        LOG_MESSAGE(LogLevel::Warn, "BiliLiveSession is stopped, on_write returning.");
        return;
    }
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
        LOG_MESSAGE(LogLevel::Warn, "BiliLiveSession is stopped, on_auth returning.");
        return;
    }
    this->ws.async_read(
        this->buffer,
        boost::beast::bind_front_handler(&BiliLiveSession::on_read, shared_from_this()));
    // 定时发送心跳包
    this->pingTimer.expires_after(std::chrono::seconds(28));
    this->pingTimer.async_wait(
        [this](const boost::system::error_code& ec) { this->ping_task(ec); });
}

void BiliLiveSession::on_read(boost::beast::error_code ec, std::size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred);
    if (ec)
    {
        // 如果错误是 operation_aborted，意味着读取被取消，这在关闭期间是预期的。
        if (ec == boost::asio::error::operation_aborted)
        {
            LOG_MESSAGE(LogLevel::Info,
                        "BiliLiveSession on_read: Operation aborted (expected during shutdown).");
        }
        else if (ec == boost::beast::websocket::error::closed)
        {
            LOG_MESSAGE(LogLevel::Info,
                        "BiliLiveSession on_read: WebSocket closed (expected during shutdown).");
        }
        else
        {
            LOG_VAR(LogLevel::Error, ec.message());
        }
        this->stopFlag.store(true);
        return;
    }
    if (this->stopFlag.load())
    {
        LOG_MESSAGE(LogLevel::Warn, "BiliLiveSession is stopped, on_read returning.");
        return;
    }
    auto                 buf     = this->buffer.data();
    auto                 bufData = buf.data();
    std::vector<uint8_t> response;
    response.assign(static_cast<const uint8_t*>(bufData),
                    static_cast<const uint8_t*>(bufData) + boost::asio::buffer_size(buf));
    this->buffer.consume(response.size());
    auto pack = BiliApiUtil::Unpack(response);
    for (auto& item : pack)
    {
        LOG_VAR(LogLevel::Debug, std::get<1>(item));
        ProcessingMessageThreadPool::GetInstance()->AddTask(std::move(item));
    }
    // 继续异步读取，但要先检查 stopFlag
    if (!this->stopFlag.load())
    {
        this->ws.async_read(
            this->buffer,
            boost::beast::bind_front_handler(&BiliLiveSession::on_read, shared_from_this()));
    }
    else
    {
        LOG_MESSAGE(LogLevel::Warn, "BiliLiveSession is stopped, not re-posting async_read.");
    }
}

void BiliLiveSession::on_close(boost::beast::error_code ec)
{
    if (ec)
    {
        // 如果错误是 operation_aborted，意味着关闭被取消，这在关闭期间是预期的。
        if (ec == boost::asio::error::operation_aborted)
        {
            LOG_MESSAGE(LogLevel::Info,
                        "BiliLiveSession on_close: Operation aborted (expected during shutdown).");
        }
        else
        {
            LOG_VAR(LogLevel::Error, "BiliLiveSession on_close error: " + ec.message());
        }
        this->stopFlag.store(true);
    }
    else
    {
        LOG_MESSAGE(LogLevel::Info, "BiliLiveSession WebSocket connection closed gracefully.");
    }
}

void BiliLiveSession::do_ping()
{
    if (this->stopFlag.load())
    {
        LOG_MESSAGE(LogLevel::Warn, "BiliLiveSession is stopped, do_ping returning.");
        return;
    }

    std::vector<uint8_t> authMessage;
    BiliApiUtil::MakePack("{}", BiliApiUtil::Operation::HEARTBEAT, authMessage);
    // Send the message
    LOG_MESSAGE(LogLevel::Debug, "do_ping");
    // 仅当 ws 连接打开时才尝试发送心跳包
    if (this->ws.is_open())
    {
        this->ws.async_write(
            boost::asio::buffer(authMessage),
            boost::beast::bind_front_handler(&BiliLiveSession::on_write, shared_from_this()));
    }
    else
    {
        LOG_MESSAGE(LogLevel::Warn, "WebSocket not open for ping, skipping.");
    }
}
void BiliLiveSession::ping_task(const boost::system::error_code& ec)
{
    if (ec)
    {
        // 如果错误是 operation_aborted，意味着定时器被取消，这在关闭期间是预期的。
        if (ec == boost::asio::error::operation_aborted)
        {
            LOG_MESSAGE(
                LogLevel::Info,
                "BiliLiveSession ping_task: Timer operation aborted (expected during shutdown).");
        }
        else
        {
            LOG_VAR(LogLevel::Error, ec.message());
        }
        return;
    }
    if (this->stopFlag.load())
    {
        LOG_MESSAGE(LogLevel::Warn, "BiliLiveSession is stopped, ping_task returning.");
        return;   // 如果停止了，就不再执行心跳任务
    }
    this->do_ping();
    // 重新设置定时器，但要先检查 stopFlag
    if (!this->stopFlag.load())
    {
        this->pingTimer.expires_after(std::chrono::seconds(28));
        this->pingTimer.async_wait([this](const boost::system::error_code& ec_next_ping) {
            if (ec_next_ping)
            {
                // 如果错误是 operation_aborted，意味着定时器被取消，这在关闭期间是预期的。
                if (ec_next_ping == boost::asio::error::operation_aborted)
                {
                    LOG_MESSAGE(LogLevel::Info,
                                "BiliLiveSession ping_task (nested): Timer operation aborted "
                                "(expected during shutdown).");
                }
                else
                {
                    LOG_VAR(LogLevel::Error, ec_next_ping.message());
                }
                return;
            }
            // 在 lambda 内部再次检查 stopFlag，以防在 async_wait 期间 stop 被调用
            if (this->stopFlag.load())
            {
                return;
            }
            this->ping_task(ec_next_ping);
        });
    }
    else
    {
        LOG_MESSAGE(LogLevel::Warn, "BiliLiveSession is stopped, not re-posting ping timer.");
    }
}
