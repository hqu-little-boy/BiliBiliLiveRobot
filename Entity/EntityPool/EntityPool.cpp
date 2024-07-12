//
// Created by zeng on 24-7-13.
//

#include "EntityPool.h"
EntityPool* EntityPool::pInstance{new EntityPool()};
EntityPool* EntityPool::GetInstance()
{
    return pInstance;
}


void EntityPool::PushCommandPool(BiliApiUtil::LiveCommand               eCommand,
                                 std::unique_ptr<BiliLiveCommandBase>&& command)
{
    std::unique_lock<std::mutex> lock{this->commandPoolMutex};
    this->commandPool[eCommand].push(std::move(command));
}

BiliLiveCommandBase* EntityPool::PopCommandPool(BiliApiUtil::LiveCommand eCommand)
{
    std::unique_lock<std::mutex> lock{this->commandPoolMutex};
    if (this->commandPool[eCommand].empty())
    {
        return nullptr;
    }
    while (!this->commandPool[eCommand].empty())
    {
        std::unique_ptr<BiliLiveCommandBase> pCommand{
            std::move(this->commandPool[eCommand].front())};
        this->commandPool[eCommand].pop();
        if (!pCommand->IsTimeOut())
        {
            return pCommand.release();
        }
    }
    return nullptr;
}

void EntityPool::ClearCommandPool()
{
    std::unique_lock<std::mutex> lock{this->commandPoolMutex};
    this->commandPool.clear();
}

bool EntityPool::IsCommandPoolEmpty(BiliApiUtil::LiveCommand eCommand)
{
    std::unique_lock<std::mutex> lock{this->commandPoolMutex};
    return this->commandPool[eCommand].empty();
}
