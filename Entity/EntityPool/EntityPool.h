//
// Created by zeng on 24-7-13.
//

#ifndef ENTITYPOOL_H
#define ENTITYPOOL_H
#include "../../Util/BiliUtil/BiliApiUtil.h"
#include "../BiliEntity/Command/BiliLiveCommandBase.h"

#include <mutex>
#include <queue>
#include <string>
#include <thread>


// 单例模式，用于存储消息队列，线程安全
class EntityPool
{
public:
    static EntityPool* GetInstance();

    void                               PushCommandPool(BiliApiUtil::LiveCommand               eCommand,
                                                       std::unique_ptr<BiliLiveCommandBase>&& commandPtr);
    [[nodiscard]] BiliLiveCommandBase* PopCommandPool(BiliApiUtil::LiveCommand eCommand);
    void                               ClearCommandPool();
    [[nodiscard]] bool                 IsCommandPoolEmpty(BiliApiUtil::LiveCommand eCommand);

private:
    EntityPool()  = default;
    ~EntityPool() = default;

    EntityPool(const EntityPool& other)            = delete;
    EntityPool(EntityPool&& other)                 = delete;
    EntityPool& operator=(const EntityPool& other) = delete;
    EntityPool& operator=(EntityPool&& other)      = delete;

    static EntityPool* pInstance;
    std::unordered_map<BiliApiUtil::LiveCommand, std::queue<std::unique_ptr<BiliLiveCommandBase>>>
               commandPool;
    std::mutex commandPoolMutex;
};



#endif   // ENTITYPOOL_H
