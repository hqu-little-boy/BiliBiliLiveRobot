//
// Created by zeng on 24-6-24.
//

#include "Entity/DataBase/DataBase.h"
#include "Entity/Global/Config.h"
#include "Entity/Global/Logger.h"
#include "Entity/Net/Multipart.h"
#include "Entity/Robot/Robot.h"
#include "Entity/ThreadPool/ProcessingMessageThreadPool.h"
#include "Util/BiliUtil/BiliLiveSession.h"
#include "Util/BiliUtil/BiliLogin.h"
#include "Util/BiliUtil/BiliRequestHeader.h"
#include "Util/BiliUtil/BiliRoomInfo.h"

#include <boost/asio/io_context.hpp>
#include <boost/beast/http/dynamic_body.hpp>
#include <cstdlib>

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        LOG_MESSAGE(
            LogLevel::Warn,
            fmt::format("Usage: {} <config_file_path> [optional]<cookie_file_path>", argv[0]));
        return EXIT_FAILURE;
    }
    // 判断
    if (!std::filesystem::exists(argv[1]))
    {
        LOG_MESSAGE(LogLevel::Error, fmt::format("Config file does not exist: {}", argv[1]));
        return EXIT_FAILURE;
    }
    std::string_view cookiePath;
    if (argc == 3)
    {
        cookiePath = argv[2];
    }
    else
    {
        cookiePath = "./cookie/bili_cookie.json";
    }
    // 判断cookie/bili_cookie.json是否存在
    if (!std::filesystem::exists(cookiePath))
    {
        BiliLogin login;
        if (!login.GetLoginQRCode())
        {
            LOG_MESSAGE(LogLevel::Error, "Failed to get login QRCode");
            return EXIT_FAILURE;
        }
    }
    else if (!BiliRequestHeader::GetInstance()->LoadBiliCookieByPath(cookiePath))
    {
        LOG_MESSAGE(LogLevel::Error, "Failed to load cookie");
        return EXIT_FAILURE;
    }
    std::string_view configPath = argv[1];
    if (!Config::GetInstance()->LoadFromJson(configPath))
    {
        LOG_MESSAGE(LogLevel::Error, "Failed to load config file");
        return EXIT_FAILURE;
    }
    Logger::GetInstance()->SetLogLevel(Config::GetInstance()->GetLogLevel());
    Logger::GetInstance()->SetLogPath(Config::GetInstance()->GetLogPath());
    LOG_MESSAGE(LogLevel::Debug, fmt::format("Config: ({})", Config::GetInstance()->ToString()));
    // ProcessingMessageThreadPool::GetInstance()->Start();
    // boost::asio::io_context ioc;
    // std::make_shared<BiliLiveSession>(ioc)->run();
    // ioc.run();
    DataBase::GetInstance()->Init();
    Robot robot;
    // robot.Run();
    BiliRoomInfo roomInfo{Config::GetInstance()->GetRoomId()};
    while (true)
    {
        if (!roomInfo.GetRoomInfo())
        {
            LOG_MESSAGE(LogLevel::Error, "Failed to get room info");
            std::this_thread::sleep_for(std::chrono::seconds(60));
            continue;
        }
        if (roomInfo.IsLiving() && !robot.IsRunning())
        {
            robot.Run();
        }
        else if (!roomInfo.IsLiving() && robot.IsRunning())
        {
            robot.Stop();
        }
        std::this_thread::sleep_for(std::chrono::seconds(60));
    }
    return EXIT_SUCCESS;
}

// int main()
// {
//     // 判断cookie/bili_cookie.json是否存在
//     std::string_view cookiePath = "./cookie/bili_cookie.json";
//     if (!std::filesystem::exists(cookiePath))
//     {
//         BiliLogin login;
//         if (!login.GetLoginQRCode())
//         {
//             LOG_MESSAGE(LogLevel::Error, "Failed to get login QRCode");
//             return EXIT_FAILURE;
//         }
//     }
//     else if (!BiliRequestHeader::GetInstance()->LoadBiliCookieByPath(cookiePath))
//     {
//         LOG_MESSAGE(LogLevel::Error, "Failed to load cookie");
//         return EXIT_FAILURE;
//     }
//
//     if (!Config::GetInstance()->LoadFromJson("./Config/configure.json"))
//     {
//         LOG_MESSAGE(LogLevel::Error, "Failed to load config file");
//         return EXIT_FAILURE;
//     }
//     Logger::GetInstance()->SetLogLevel(Config::GetInstance()->GetLogLevel());
//     Logger::GetInstance()->SetLogPath(Config::GetInstance()->GetLogPath());
//     LOG_MESSAGE(LogLevel::Debug, fmt::format("Config: ({})", Config::GetInstance()->ToString()));
//
//     Url url{"api.live.bilibili.com", 443, "/msg/send"};
//     LOG_VAR(LogLevel::Debug, url.ToString());
//
//     boost::asio::io_context        ioc;                                             // IO上下文
//     boost::asio::ssl::context      ctx(boost::asio::ssl::context::tlsv12_client);   // SSL上下文
//     boost::asio::ip::tcp::resolver resolver(ioc);                                   // DNS解析器
//
//     // 解析域名和端口
//     const auto results = resolver.resolve(url.GetHost(), std::to_string(url.GetPort()));

//     // 创建SSL流并连接
//     boost::asio::io_context                                iocHttp;
//     boost::asio::ssl::stream<boost::asio::ip::tcp::socket> stream(iocHttp, ctx);
//     boost::asio::connect(stream.next_layer(), results.begin(), results.end());
//     stream.handshake(boost::asio::ssl::stream_base::client);
//     Multipart multipart{
//         {"bubble", "5"},
//         {"msg", "圈宝，可爱"},
//         {"color", "4546550"},
//         // {"mode", "1"},
//         {"room_type", "0"},
//         // {"jumpfrom", "71002"},
//         // {"reply_mid", "0"},
//         // {"reply_attr", "0"},
//         // {"replay_dmid", ""},
//         // {"statistics", "{\"appId\":100,\"platform\":5}"},
//         {"fontsize", "25"},
//         {"rnd", std::to_string(std::time(nullptr))},
//         {"roomid", std::to_string(Config::GetInstance()->GetRoomId())},
//         {"csrf", BiliRequestHeader::GetInstance()->GetBiliCookie().GetBiliJct()},
//         {"csrf_token", BiliRequestHeader::GetInstance()->GetBiliCookie().GetBiliJct()}};
//     // 构建POST请求
//     boost::beast::http::request<boost::beast::http::string_body> req{
//         boost::beast::http::verb::post, url.GetTarget(), 11};
//     req.set(boost::beast::http::field::host, url.GetHost());
//     req.set(boost::beast::http::field::user_agent,
//             BiliRequestHeader::GetInstance()->GetUserAgent());
//     req.set(boost::beast::http::field::content_type,
//             multipart.GetSerializeMultipartFormdataGetContentType());
//     // 设置cookie
//     req.set(boost::beast::http::field::cookie,
//             BiliRequestHeader::GetInstance()->GetBiliCookie().ToString());
//     //
//     req.body() = multipart.GetSerializeMultipartFormdata();
//     req.prepare_payload();
//     // 发送请求
//     boost::beast::http::write(stream, req);
//
//
//     // This buffer is used for reading and must be persisted
//     boost::beast::flat_buffer buffer;
//
//     // Declare a container to hold the response
//     boost::beast::http::response<boost::beast::http::dynamic_body> res;
//
//     // Receive the HTTP response
//     boost::beast::http::read(stream, buffer, res);
//
//     // Write the message to standard out
//     std::cout << res << std::endl;
//
//     // Gracefully close the socket
//     boost::beast::error_code ec;
//
//     return true;
// }