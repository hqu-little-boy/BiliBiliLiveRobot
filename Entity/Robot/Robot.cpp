//
// Created by zpf on 25-3-12.
//

#include "Robot.h"

#include "../../Util/BiliUtil/BiliLiveSession.h"
#include "../Global/Config.h"
#include "../ThreadPool/ProcessingMessageThreadPool.h"

#include <boost/asio/io_context.hpp>
Robot::Robot()
    : stopFlag{true}
    , liveSessionPtr{nullptr}
{
}

void Robot::Run()
{
    LOG_MESSAGE(LogLevel::Info, "Robot::Run");
    if (!this->stopFlag.load())
    {
        LOG_MESSAGE(LogLevel::Error, "Robot is already running");
        return;
    }
    if (!Config::GetInstance()->LoadUID())
    {
        LOG_MESSAGE(LogLevel::Error, "Failed to load UID");
        return;
    }
    this->stopFlag.store(false);
    this->robotThread = std::thread(&Robot::RobotThread, this);
}

void Robot::Stop()
{
    LOG_MESSAGE(LogLevel::Info, "Robot::Stop");
    this->stopFlag.store(true);
    if (ProcessingMessageThreadPool::GetInstance()->IsRunning())
    {
        ProcessingMessageThreadPool::GetInstance()->Stop();
    }
    else
    {
        LOG_MESSAGE(LogLevel::Error, "Thread pool is not running");
    }

    if (this->liveSessionPtr)
    {
        this->liveSessionPtr->stop();
        this->liveSessionPtr.reset();
    }
    else
    {
        LOG_MESSAGE(LogLevel::Error, "liveSessionPtr is nullptr");
    }

    if (!this->ioc.stopped())
    {
        this->ioc.stop();
    }
    else
    {
        LOG_MESSAGE(LogLevel::Error, "ioc is stopped");
    }

    if (this->robotThread.joinable())
    {
        this->robotThread.join();
    }
    else
    {
        LOG_MESSAGE(LogLevel::Error, "robotThread is not joinable");
    }
    this->ioc.restart();
}

bool Robot::IsRunning() const
{
    return !this->stopFlag.load();
}

void Robot::RobotThread()
{
    LOG_MESSAGE(LogLevel::Info, "Robot::RobotThread");
    if (this->stopFlag.load())
    {
        LOG_MESSAGE(LogLevel::Error, "Robot is stopped");
        return;
    }
    if (ProcessingMessageThreadPool::GetInstance()->IsRunning())
    {
        LOG_MESSAGE(LogLevel::Error, "Thread pool is already running");
        return;
    }
    ProcessingMessageThreadPool::GetInstance()->Start();
    if (this->liveSessionPtr)
    {
        LOG_MESSAGE(LogLevel::Error, "liveSessionPtr is not nullptr");
        return;
    }
    // this->iocPtr         = std::make_unique<boost::asio::io_context>();
    this->liveSessionPtr = std::make_shared<BiliLiveSession>(this->ioc);
    bool res{this->liveSessionPtr->run()};
    if (!res)
    {
        LOG_MESSAGE(LogLevel::Error, "Failed to run live session");
        this->Stop();
        return;
    }
    auto work = boost::asio::make_work_guard(ioc);
    this->ioc.run();
}