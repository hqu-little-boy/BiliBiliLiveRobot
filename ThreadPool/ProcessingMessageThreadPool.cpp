//
// Created by zeng on 24-7-7.
//
#include "ProcessingMessageThreadPool.h"

#include "../Entity/BiliEntity/BiliCommandFactory.h"
#include "../Entity/Global/Logger.h"
#include "../Entity/Global/MessageDeque.h"
#include "../Util/BiliUtil/BiliApiUtil.h"
ProcessingMessageThreadPool* ProcessingMessageThreadPool::pInstance{
    new ProcessingMessageThreadPool()};
const int                    ProcessingMessageThreadPool::threadNum{static_cast<int>(
    std::thread::hardware_concurrency() > 2 ? std::thread::hardware_concurrency() / 2 : 1)};
ProcessingMessageThreadPool* ProcessingMessageThreadPool::GetInstance()
{
    if (pInstance == nullptr)
    {
        assert(false);
        pInstance = new ProcessingMessageThreadPool();
    }
    return pInstance;
}

void ProcessingMessageThreadPool::AddTask(std::string&& message)
{
    std::unique_lock<std::mutex> lock(this->processingMessageThreadPoolMutex);
    this->taskQueue.push(message);
    this->processingMessageThreadPoolCondition.notify_one();
}

void ProcessingMessageThreadPool::Start()
{
    for (int i = 0; i < threadNum; ++i)
    {
        this->processingMessageThreadPool.emplace_back(&ProcessingMessageThreadPool::ThreadRun,
                                                       this);
    }
}

void ProcessingMessageThreadPool::ThreadRun()
{
    std::unique_lock<std::mutex> lock(this->processingMessageThreadPoolMutex);
    while (true)
    {
        this->processingMessageThreadPoolCondition.wait(
            lock, [this]() { return !this->taskQueue.empty(); });
        std::string message = std::move(this->taskQueue.front());
        this->taskQueue.pop();
        lock.unlock();
        auto command = BiliApiUtil::GetLiveCommand(message);
        if (command == BiliApiUtil::LiveCommand::NONE || command == BiliApiUtil::LiveCommand::OTHER)
        {
            assert(false);
            LOG_MESSAGE(LogLevel::Error, "Invalid command");
            return;
        }
        nlohmann::json jsonMessage;
        try
        {
            jsonMessage = nlohmann::json::parse(message);
        }
        catch (const nlohmann::json::parse_error& e)
        {
            LOG_MESSAGE(LogLevel::Error, e.what());
            return;
        }
        auto uniqueCommand =
            std::move(BiliCommandFactory::GetInstance()->GetCommand(command, jsonMessage));
        if (uniqueCommand == nullptr)
        {
            LOG_MESSAGE(LogLevel::Error,
                        BiliApiUtil::GetLiveCommandStr(message) + " Failed to produce command");
            return;
        }
        uniqueCommand->Run();
        uniqueCommand->SetTimeStamp();
        MessageDeque::GetInstance()->PushCommandPool(uniqueCommand->GetCommandType(), std::move(uniqueCommand));
        lock.lock();
    }
}