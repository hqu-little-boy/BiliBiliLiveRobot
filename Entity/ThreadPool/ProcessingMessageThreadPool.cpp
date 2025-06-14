//
// Created by zeng on 24-7-7.
//
#include "ProcessingMessageThreadPool.h"

#include "../../Util/BiliUtil/BiliApiUtil.h"
#include "../BiliEntity/BiliCommandFactory.h"
#include "../EntityPool/EntityPool.h"
#include "../Global/Logger.h"
#include "../MessageDeque/MessageDeque.h"
ProcessingMessageThreadPool* ProcessingMessageThreadPool::pInstance{
    new ProcessingMessageThreadPool()};
int ProcessingMessageThreadPool::threadNum =
    static_cast<int>((std::thread::hardware_concurrency() + 3) / 4);
ProcessingMessageThreadPool* ProcessingMessageThreadPool::GetInstance()
{
    if (pInstance == nullptr)
    {
        assert(false);
        pInstance = new ProcessingMessageThreadPool();
    }
    return pInstance;
}

void ProcessingMessageThreadPool::AddTask(
    std::tuple<BiliApiUtil::LiveCommand, std::string>&& message)
{
    std::unique_lock<std::mutex> lock(this->processingMessageThreadPoolMutex);
    this->taskQueue.push(message);
    this->processingMessageThreadPoolCondition.notify_one();
}

ProcessingMessageThreadPool::ProcessingMessageThreadPool()
{
    ProcessingMessageThreadPool::threadNum =
        static_cast<int>((std::thread::hardware_concurrency() + 3) / 4);
    LOG_VAR(LogLevel::Debug, threadNum);
    LOG_MESSAGE(LogLevel::Info, "ProcessingMessageThreadPool::ProcessingMessageThreadPool");
    for (int i = 0; i < threadNum; ++i)
    {
        this->processingMessageThreadPool.emplace_back(&ProcessingMessageThreadPool::ThreadRun,
                                                       this);
    }
}

void ProcessingMessageThreadPool::ThreadRun()
{
    while (true)
    {
        std::tuple<BiliApiUtil::LiveCommand, std::string> message;
        // 及时释放锁
        {
            std::unique_lock<std::mutex> lock(this->processingMessageThreadPoolMutex);
            if (this->taskQueue.empty())
            {
                this->processingMessageThreadPoolCondition.wait(
                    lock, [this]() { return !this->taskQueue.empty(); });
            }
            message = std::move(this->taskQueue.front());
            this->taskQueue.pop();
        }
        nlohmann::json jsonMessage;
        try
        {
            jsonMessage = nlohmann::json::parse(std::get<1>(message));
        }
        catch (const nlohmann::json::parse_error& e)
        {
            LOG_MESSAGE(LogLevel::Error, e.what());
            continue;
        }
        auto uniqueCommand = std::move(
            BiliCommandFactory::GetInstance()->GetCommand(std::get<0>(message), jsonMessage));
        if (uniqueCommand == nullptr)
        {
            LOG_MESSAGE(LogLevel::Error,
                        BiliApiUtil::GetLiveCommandStr(std::get<1>(message)) +
                            " Failed to produce command");
            continue;
        }
        uniqueCommand->Run();
        uniqueCommand->SetTimeStamp();
        EntityPool::GetInstance()->PushCommandPool(uniqueCommand->GetCommandType(),
                                                   std::move(uniqueCommand));
    }
}