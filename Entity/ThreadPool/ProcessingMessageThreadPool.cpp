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
const int ProcessingMessageThreadPool::threadNum{
    static_cast<int>((std::thread::hardware_concurrency() + 15) / 16)};
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

void ProcessingMessageThreadPool::Start()
{
    if (!this->processingMessageThreadPool.empty() || !this->stopFlag.load())
    {
        LOG_MESSAGE(LogLevel::Error, "Thread pool is already running");
        return;
    }

    this->stopFlag.store(false);
    for (int i = 0; i < threadNum; ++i)
    {
        this->processingMessageThreadPool.emplace_back(&ProcessingMessageThreadPool::ThreadRun,
                                                       this);
    }
}

void ProcessingMessageThreadPool::Stop()
{
    this->stopFlag.store(true);
    EntityPool::GetInstance()->ClearCommandPool();
    for (auto& thread : this->processingMessageThreadPool)
    {
        thread.join();
    }
    this->processingMessageThreadPool.clear();
}

bool ProcessingMessageThreadPool::IsRunning() const
{
    return !this->stopFlag.load();
}

ProcessingMessageThreadPool::ProcessingMessageThreadPool()
    : stopFlag{true}
{
}

void ProcessingMessageThreadPool::ThreadRun()
{
    while (true)
    {
        if (this->stopFlag.load())
        {
            break;
        }

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