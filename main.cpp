//
// Created by zeng on 24-6-24.
//

#include "Entity/Global/Config.h"
#include "Entity/Global/Logger.h"
#include "Entity/Net/Multipart.h"
#include "Entity/Robot/Robot.h"
#include "Entity/ThreadPool/ProcessingMessageThreadPool.h"
#include "Util/BiliUtil/BiliLiveSession.h"
#include "Util/BiliUtil/BiliLogin.h"
#include "Util/BiliUtil/BiliRequestHeader.h"
#include "Util/BiliUtil/BiliRoomInfo.h"

#include <QApplication>
#include <QDebug>
#include <QtQuick>
#include <boost/asio/io_context.hpp>
#include <boost/beast/http/dynamic_body.hpp>
#include <cstdlib>

bool InitRobot()
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
    LOG_MESSAGE(LogLevel::Debug, fmt::format("Config: ({})", Config::GetInstance()->ToString()));
    // ProcessingMessageThreadPool::GetInstance()->Start();
    // boost::asio::io_context ioc;
    // std::make_shared<BiliLiveSession>(ioc)->run();
    // ioc.run();
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
    return true;
}

int main(int argc, char* argv[])
{
    QApplication          app(argc, argv);
    QQmlApplicationEngine engine;
    engine.addImportPath("qrc:/");
    const QUrl mainUrl(QStringLiteral("qrc:/main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [mainUrl](QObject* obj, const QUrl& objUrl) {
            if (!obj && mainUrl == objUrl)
            {
                qDebug() << "qml engine load failed";
                QCoreApplication::exit(-1);
            }
        },
        Qt::QueuedConnection);
    engine.load(mainUrl);
    return app.exec();
}
