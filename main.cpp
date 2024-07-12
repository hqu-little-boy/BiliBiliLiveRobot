//
// Created by zeng on 24-6-24.
//

#include "Entity/Global/Config.h"
#include "Entity/Global/Logger.h"
#include "Entity/ThreadPool/ProcessingMessageThreadPool.h"
#include "Util/BiliUtil/BiliLiveSession.h"
#include "Util/BiliUtil/BiliLogin.h"
#include "Util/BiliUtil/BiliRequestHeader.h"

#include <boost/asio/io_context.hpp>
#include <cstdlib>

int main()
{
    // 判断cookie/bili_cookie.json是否存在
    std::string_view cookiePath = "./cookie/bili_cookie.json";
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

    if (!Config::GetInstance()->LoadFromJson("./Config/configure.json"))
    {
        LOG_MESSAGE(LogLevel::Error, "Failed to load config file");
        return EXIT_FAILURE;
    }
    Logger::GetInstance()->SetLogLevel(Config::GetInstance()->GetLogLevel());
    Logger::GetInstance()->SetLogPath(Config::GetInstance()->GetLogPath());
    LOG_MESSAGE(LogLevel::Debug, std::format("Config: ({})", Config::GetInstance()->ToString()));
    ProcessingMessageThreadPool::GetInstance()->Start();
    boost::asio::io_context ioc;
    std::make_shared<BiliLiveSession>(ioc)->run();
    ioc.run();
    // while (true)
    // {
    // }
    return EXIT_SUCCESS;
}
