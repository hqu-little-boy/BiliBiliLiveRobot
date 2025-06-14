//
// Created by zpf on 25-3-12.
//

#include "Robot.h"

#include "../../Util/BiliUtil/BiliLiveSession.h"
#include "../Global/Config.h"
#include "../ThreadPool/ProcessingMessageThreadPool.h"

#include <boost/asio/io_context.hpp>
#include <memory>   // Make sure <memory> is included for std::shared_ptr
#include <thread>   // Make sure <thread> is included for std::thread

Robot::Robot()
    : stopFlag{true}
    , liveSessionPtr{nullptr}
{
}
Robot::~Robot()
{
    LOG_MESSAGE(LogLevel::Info, "Robot::~Robot");
    if (!this->stopFlag.load())
    {
        LOG_MESSAGE(LogLevel::Error, "Robot is still running, stopping it now.");
        this->Stop();   // 确保在析构时停止机器人
    }
    if (this->robotThread.joinable())
    {
        this->robotThread.join();   // 等待线程结束
    }
    LOG_MESSAGE(LogLevel::Info, "Robot destroyed successfully");
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
    this->stopFlag.store(true);   // 设置 Robot 的停止标志

    if (this->liveSessionPtr)
    {
        LOG_MESSAGE(LogLevel::Info, "Stopping live session");
        this->liveSessionPtr->stop();   // 调用 BiliLiveSession 的 stop 方法
    }
    else
    {
        LOG_MESSAGE(LogLevel::Error, "liveSessionPtr is nullptr");
    }

    // 等待 robotThread 完成，确保在释放资源前线程已安全退出。
    if (this->robotThread.joinable())
    {
        LOG_MESSAGE(LogLevel::Info, "Joining robotThread");
        this->robotThread.join();
    }
    else
    {
        LOG_MESSAGE(LogLevel::Error, "robotThread is not joinable");
    }

    // 在线程加入且 io_context 停止后，现在可以安全地释放 shared_ptr。
    if (this->liveSessionPtr)
    {
        this->liveSessionPtr.reset();
        // 确保 liveSessionPtr 被重置
        LOG_MESSAGE(LogLevel::Info, "liveSessionPtr reset.");
    }

    // 重启 io_context 以备将来可能的使用。
    LOG_MESSAGE(LogLevel::Info, "Robot stopped successfully");
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
    if (this->liveSessionPtr)
    {
        LOG_MESSAGE(LogLevel::Error, "liveSessionPtr is not nullptr");
        return;
    }
    this->liveSessionPtr = std::make_shared<BiliLiveSession>();
    bool res{this->liveSessionPtr->run()};
    if (!res)
    {
        LOG_MESSAGE(LogLevel::Error, "Failed to run live session");
        this->Stop();   // 如果 BiliLiveSession::run() 失败，则调用 Robot::Stop() 进行清理
        return;
    }
    LOG_MESSAGE(LogLevel::Info, "Robot::RobotThread io_context::run() returned successfully");
}
