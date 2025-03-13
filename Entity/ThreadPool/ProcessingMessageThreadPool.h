//
// Created by zeng on 24-7-7.
//

#ifndef PROCESSINGMESSAGETHREADPOOL_H
#define PROCESSINGMESSAGETHREADPOOL_H
#include "../BiliEntity/Command/BiliLiveCommandBase.h"

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
    void AddTask(std::tuple<BiliApiUtil::LiveCommand, std::string>&& message);
    void Start();
    void Stop();
    bool IsRunning() const;
private:
    ProcessingMessageThreadPool();
    ~ProcessingMessageThreadPool()                                             = default;
    ProcessingMessageThreadPool(const ProcessingMessageThreadPool&)            = delete;
    ProcessingMessageThreadPool& operator=(const ProcessingMessageThreadPool&) = delete;

    void                                                          ThreadRun();
    static ProcessingMessageThreadPool*                           pInstance;
    std::queue<std::tuple<BiliApiUtil::LiveCommand, std::string>> taskQueue;
    std::list<std::thread>                                        processingMessageThreadPool;
    std::mutex                                                    processingMessageThreadPoolMutex;
    std::condition_variable processingMessageThreadPoolCondition;
    std::atomic<bool>       stopFlag;
    static const int        threadNum;
};



#endif   // PROCESSINGMESSAGETHREADPOOL_H
