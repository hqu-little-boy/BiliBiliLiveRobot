//
// Created by zeng on 24-7-6.
//

#ifndef MESSAGEDEQUE_H
#define MESSAGEDEQUE_H
#include "../../Util/BiliUtil/BiliApiUtil.h"
#include "../BiliEntity/Command/BiliLiveCommandBase.h"
#include "../Net/Url.h"
#include "../Global/PlatformDefine.h"

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/context.hpp>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <string>
#include <thread>


// 单例模式，用于存储消息队列，线程安全
class MessageDeque
{
public:
    static MessageDeque *GetInstance();
    void                 PushWaitedMessage(const std::string &message);
    // [[nodiscard]] std::string PopWaitedMessage();
    void ClearWaitedMessage();

private:
    void SendMessageInThread();

    bool SendMessageToBili(const std::string &message);

    MessageDeque();
    ~MessageDeque() = default;

    MessageDeque(const MessageDeque &other)            = delete;
    MessageDeque(MessageDeque &&other)                 = delete;
    MessageDeque &operator=(const MessageDeque &other) = delete;
    MessageDeque &operator=(MessageDeque &&other)      = delete;

    static MessageDeque    *pInstance;
    std::queue<std::string> messageQueue;
    std::mutex              messageQueueMutex;
    std::condition_variable messageQueueCondtionVariable;

    const static Url url;

    THREAD                   snedMessageThread;
    boost::asio::io_context        ioc;        // IO上下文
    boost::asio::ssl::context      ctx;        // SSL上下文
    boost::asio::ip::tcp::resolver resolver;   // DNS解析器
};



#endif   // MESSAGEDEQUE_H
