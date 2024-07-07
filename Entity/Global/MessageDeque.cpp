//
// Created by zeng on 24-7-6.
//

#include "MessageDeque.h"

MessageDeque* MessageDeque::pInstance{new MessageDeque()};
MessageDeque* MessageDeque::GetInstance()
{
    return pInstance;
}

void MessageDeque::PushWaitedMessage(const std::string& message)
{
    std::unique_lock<std::mutex> lock{this->messageQueueMutex};
    this->messageQueue.push(message);
    this->messageQueueCondtionVariable.notify_one();
}

std::string MessageDeque::PopWaitedMessage()
{
    std::unique_lock<std::mutex> lock{this->messageQueueMutex};
    this->messageQueueCondtionVariable.wait(lock, [this] { return !this->messageQueue.empty(); });
    std::string message{std::move(this->messageQueue.front())};
    this->messageQueue.pop();
    return message;
}

void MessageDeque::ClearWaitedMessage()
{
    std::unique_lock<std::mutex> lock{this->messageQueueMutex};
    while (!this->messageQueue.empty())
    {
        this->messageQueue.pop();
    }
}

void MessageDeque::PushCommandPool(BiliApiUtil::LiveCommand           eCommand,
                                   std::unique_ptr<BiliLiveCommandBase>&& command)
{
    std::unique_lock<std::mutex> lock{this->commandPoolMutex};
    this->commandPool[eCommand].push(std::move(command));
}

BiliLiveCommandBase* MessageDeque::PopCommandPool(BiliApiUtil::LiveCommand eCommand)
{
    std::unique_lock<std::mutex> lock{this->commandPoolMutex};
    if (this->commandPool[eCommand].empty())
    {
        return nullptr;
    }
    while (!this->commandPool[eCommand].empty())
    {
        std::unique_ptr<BiliLiveCommandBase> pCommand{std::move(this->commandPool[eCommand].front())};
        this->commandPool[eCommand].pop();
        if (!pCommand->IsTimeOut())
        {
            return pCommand.release();
        }
    }
    return nullptr;
}

void MessageDeque::ClearCommandPool()
{
    std::unique_lock<std::mutex> lock{this->commandPoolMutex};
    this->commandPool.clear();
}