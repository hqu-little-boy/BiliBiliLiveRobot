//
// Created by zeng on 24-6-24.
//

#include "Base/Logger.h"
#include "Entity/BiliLiveSession.h"
#include "Entity/Config.h"

#include <boost/asio/io_context.hpp>
#include <cstdlib>
int main()
{
    if (!Config::GetInstance()->LoadFromJson(
            "/home/zeng/CLionProjects/BiliBiliLiveRobot/Config/configure.json"))
    {
        LOG_MESSAGE(LogLevel::ERROR, "Failed to load config file");
        return EXIT_FAILURE;
    }
    Logger::GetInstance()->SetLogLevel(Config::GetInstance()->GetLogLevel());
    Logger::GetInstance()->SetLogPath(Config::GetInstance()->GetLogPath());
    LOG_MESSAGE(LogLevel::DEBUG, std::format("Config: ({})", Config::GetInstance()->ToString()));
    boost::asio::io_context ioc;
    std::make_shared<BiliLiveSession>(ioc)->run();
    ioc.run();
    // while (true)
    // {
    // }
    return EXIT_SUCCESS;
}