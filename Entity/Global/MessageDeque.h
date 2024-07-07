//
// Created by zeng on 24-7-6.
//

#ifndef MESSAGEDEQUE_H
#define MESSAGEDEQUE_H
#include "../../Util/BiliUtil/BiliApiUtil.h"
#include "../BiliEntity/Command/BiliLiveCommandBase.h"

#include <condition_variable>
#include <mutex>
#include <queue>
#include <string>


// 单例模式，用于存储消息队列，线程安全
class MessageDeque
{
public:
    static MessageDeque*      GetInstance();
    void                      PushWaitedMessage(const std::string& message);
    [[nodiscard]] std::string PopWaitedMessage();
    void                      ClearWaitedMessage();

    void                           PushCommandPool(BiliApiUtil::LiveCommand           eCommand,
                                                   std::unique_ptr<BiliLiveCommandBase>&& commandPtr);
    [[nodiscard]] BiliLiveCommandBase* PopCommandPool(BiliApiUtil::LiveCommand eCommand);
    void                           ClearCommandPool();

private:
    MessageDeque()  = default;
    ~MessageDeque() = default;

    MessageDeque(const MessageDeque& other)            = delete;
    MessageDeque(MessageDeque&& other)                 = delete;
    MessageDeque& operator=(const MessageDeque& other) = delete;
    MessageDeque& operator=(MessageDeque&& other)      = delete;

    static MessageDeque*    pInstance;
    std::queue<std::string> messageQueue;
    std::mutex              messageQueueMutex;
    std::condition_variable messageQueueCondtionVariable;

    std::unordered_map<BiliApiUtil::LiveCommand, std::queue<std::unique_ptr<BiliLiveCommandBase>>>
               commandPool;
    std::mutex commandPoolMutex;
};



#endif   // MESSAGEDEQUE_H
