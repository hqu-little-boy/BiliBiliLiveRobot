//
// Created by zeng on 24-6-25.
//

#include "BiliLiveSession.h"

#include "../../Entity/Global/Config.h"
#include "../../Entity/Global/Logger.h"
#include "../../ThreadPool/ProcessingMessageThreadPool.h"
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
{
}

bool BiliLiveSession::InitSSLCert()
{
    // 判断证书是否存在
    if (!std::filesystem::exists("/etc/ssl/certs/ca-certificates.crt"))
    {
        LOG_MESSAGE(LogLevel::ERROR, "Failed to find ssl cert");
        return false;
    }
    // 加载证书
    this->ctx.load_verify_file("/etc/ssl/certs/ca-certificates.crt");
    // 设置证书验证模式
    this->ctx.set_verify_mode(boost::asio::ssl::verify_peer);
    return true;
}

bool BiliLiveSession::InitRoomInfo()
{
    LOG_VAR(LogLevel::DEBUG, Config::GetInstance()->GetDanmuSeverConfUrl().GetHost());
    LOG_VAR(LogLevel::DEBUG, Config::GetInstance()->GetDanmuSeverConfUrl().GetPort());
    LOG_VAR(LogLevel::DEBUG, Config::GetInstance()->GetDanmuSeverConfUrl().GetTarget());
    LOG_VAR(LogLevel::DEBUG, Config::GetInstance()->GetDanmuSeverConfUrl().GetTargetWithQuery());
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
        Config::GetInstance()->GetDanmuSeverConfUrl().GetTargetWithQuery(),
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
        LOG_VAR(LogLevel::ERROR, res.result_int());
        return false;
    }
    // 解析json
    std::string    resStr        = boost::beast::buffers_to_string(res.body().data());
    nlohmann::json danmuInfoJson = nlohmann::json::parse(resStr);
    LOG_VAR(LogLevel::DEBUG, danmuInfoJson.dump(4));
    if (danmuInfoJson["code"] != 0)
    {
        LOG_VAR(LogLevel::ERROR, danmuInfoJson["message"].dump(4));
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

void BiliLiveSession::run()
{
    if (!this->InitSSLCert())
    {
        LOG_MESSAGE(LogLevel::ERROR, "Failed to init ssl cert");
        return;
    }
    if (!this->InitRoomInfo())
    {
        LOG_MESSAGE(LogLevel::ERROR, "Failed to init room info");
        return;
    }
    // 异步解析域名
    this->resolver.async_resolve(
        this->liveUrls.back().host,
        std::to_string(this->liveUrls.back().wss_port),
        boost::beast::bind_front_handler(&BiliLiveSession::on_resolve, shared_from_this()));
    // this->ws.async_read(
    //     this->buffer,
    //     boost::beast::bind_front_handler(&BiliLiveSession::on_read, shared_from_this()));
}

void BiliLiveSession::on_resolve(boost::beast::error_code                            ec,
                                 const boost::asio::ip::tcp::resolver::results_type& results)
{
    // 如果有错误则返回错误信息
    if (ec)
    {
        LOG_VAR(LogLevel::ERROR, ec.message());
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
        LOG_VAR(LogLevel::ERROR, ec.message());
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
        LOG_VAR(LogLevel::ERROR, ec.message());
        return;
    }

    // Update the host_ string. This will provide the value of the
    // Host HTTP header during the WebSocket handshake.
    // See https://tools.ietf.org/html/rfc7230#section-5.4
    this->host = std::format("{}:{}", this->liveUrls.back().host, ep.port());

    // Perform the SSL handshake
    this->ws.next_layer().async_handshake(
        boost::asio::ssl::stream_base::client,
        boost::beast::bind_front_handler(&BiliLiveSession::on_ssl_handshake, shared_from_this()));
}

void BiliLiveSession::on_ssl_handshake(boost::beast::error_code ec)
{
    if (ec)
    {
        LOG_VAR(LogLevel::ERROR, ec.message());
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
        LOG_VAR(LogLevel::ERROR, ec.message());
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
    // LOG_VAR(LogLevel::DEBUG, body.dump(4));
    // LOG_VAR(LogLevel::DEBUG, body.dump(-1));
    // std::string bodyStr = R"({"roomid": )" + std::to_string(Config::GetInstance()->GetRoomId()) +
    //                       R"(, "key": ")" + this->token + R"("})";
    // std::string bodyStr =
    //     "{" +
    //     std::format(
    //         R"("uid": {}, "roomid": {}, "protover": 3, "platform": "web", "type": 2, "key": "{}",
    //         "buvid": {})",
    //         // BiliRequestHeader::GetInstance()->GetBiliCookie().GetDedeUserID(),
    //         0,
    //         Config::GetInstance()->GetRoomId(),
    //         this->token,
    //         BiliRequestHeader::GetInstance()->GetBiliCookie().GetBuvid3()) +
    //     "}";
    // LOG_VAR(LogLevel::DEBUG, bodyStr);
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
        LOG_VAR(LogLevel::ERROR, ec.message());
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
    this->ws.async_read(
        this->buffer,
        boost::beast::bind_front_handler(&BiliLiveSession::on_read, shared_from_this()));
    // 定时发送心跳包
    this->pingThread = std::move(std::jthread(&BiliLiveSession::do_ping, this));
    this->pingThread.detach();
}

void BiliLiveSession::on_read(boost::beast::error_code ec, std::size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred);

    if (ec)
    {
        // std::string bodyStr = R"({})";
        // // body一行输出
        // // std::this_thread::sleep_for(std::chrono::seconds(20));
        // auto authMessage = BiliApiUtil::MakePack(bodyStr, BiliApiUtil::Operation::HEARTBEAT);
        // // Send the message
        // this->ws.async_write(
        //     authMessage,
        //     boost::beast::bind_front_handler(&BiliLiveSession::on_write, shared_from_this()));
        LOG_VAR(LogLevel::ERROR, ec.message());
        return;
    }
    // std::cout << boost::beast::make_printable(this->buffer.data()) << std::endl;
    // LOG_VAR(LogLevel::DEBUG, std::string(boost::beast::buffers_to_string(this->buffer.data())));
    // 将this->buffer.data()转为std::vector<uint8_t>
    auto                 buf = this->buffer.data();
    std::vector<uint8_t> response;
    response.assign(boost::asio::buffer_cast<const uint8_t*>(buf),
                    boost::asio::buffer_cast<const uint8_t*>(buf) + boost::asio::buffer_size(buf));
    this->buffer.consume(response.size());
    auto pack = BiliApiUtil::Unpack(response);
    for (auto& [command, content] : pack)
    {
        // LOG_VAR(LogLevel::INFO, std::get<0>(item).ToString());
        // LOG_VAR(LogLevel::DEBUG, item);
        // std::regex r(R"(\\)");                                  // 正则表达式匹配'\'
        // std::string result = std::regex_replace(item, r, "");   // 用空字符串替换所有匹配的'\'
        // try
        // {
        //     nlohmann::json json = nlohmann::json::parse(content);
        //     LOG_VAR(LogLevel::DEBUG, json.dump(-1));
        // }
        // catch (const nlohmann::json::exception& e)
        // {
        //     LOG_VAR(LogLevel::ERROR, e.what());
        //     LOG_VAR(LogLevel::ERROR, content);
        //     // LOG_VAR(LogLevel::ERROR, result);
        // }
        LOG_VAR(LogLevel::DEBUG, content);
        ProcessingMessageThreadPool::GetInstance()->AddTask(std::move(content));
    }
    this->ws.async_read(
        this->buffer,
        boost::beast::bind_front_handler(&BiliLiveSession::on_read, shared_from_this()));
}

void BiliLiveSession::on_close(boost::beast::error_code ec)
{
    if (ec)
    {
        LOG_VAR(LogLevel::ERROR, ec.message());
        return;
    }
    // If we get here then the connection is closed gracefully

    // The make_printable() function helps print a ConstBufferSequence
    // std::cout << boost::beast::make_printable(this->buffer.data()) << std::endl;
    this->pingThread.request_stop();   // 停止心跳线程
}

void BiliLiveSession::do_ping()
{
    for (;;)
    {
        std::vector<uint8_t> authMessage;
        BiliApiUtil::MakePack("{}", BiliApiUtil::Operation::HEARTBEAT, authMessage);
        // Send the message
        this->ws.async_write(
            boost::asio::buffer(authMessage),
            boost::beast::bind_front_handler(&BiliLiveSession::on_write, shared_from_this()));
        // this->ws.write(boost::asio::buffer(authMessage));
        std::this_thread::sleep_for(std::chrono::seconds(29));
    }
}