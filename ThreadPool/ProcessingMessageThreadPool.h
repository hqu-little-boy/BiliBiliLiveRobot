//
// Created by zeng on 24-7-7.
//

#ifndef PROCESSINGMESSAGETHREADPOOL_H
#define PROCESSINGMESSAGETHREADPOOL_H
#include "../Entity/BiliEntity/Command/BiliLiveCommandBase.h"

#include <condition_variable>
#include <deque>
#include <list>
#include <memory>
#include <queue>
#include <thread>
// 线程池，用于处理消息，单例模式
class ProcessingMessageThreadPool
{
public:
    static ProcessingMessageThreadPool* GetInstance();
    void                                AddTask(const std::string& message);
    void                                Start();

private:
    ProcessingMessageThreadPool()                                              = default;
    ~ProcessingMessageThreadPool()                                             = default;
    ProcessingMessageThreadPool(const ProcessingMessageThreadPool&)            = delete;
    ProcessingMessageThreadPool& operator=(const ProcessingMessageThreadPool&) = delete;

    void                                             ThreadRun();
    static ProcessingMessageThreadPool*              pInstance;
    std::queue<std::unique_ptr<BiliLiveCommandBase>> taskQueue;
    std::list<std::jthread>                          processingMessageThreadPool;
    std::mutex                                       processingMessageThreadPoolMutex;
    std::condition_variable                          processingMessageThreadPoolCondition;
    static const int                                 threadNum;
};



#endif   // PROCESSINGMESSAGETHREADPOOL_H
