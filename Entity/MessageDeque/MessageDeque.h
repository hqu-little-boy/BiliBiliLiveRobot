//
// Created by zeng on 24-7-6.
//

#ifndef MESSAGEDEQUE_H
#define MESSAGEDEQUE_H
#include "../../Util/BiliUtil/BiliApiUtil.h"
#include "../BiliEntity/Command/BiliLiveCommandBase.h"
#include "../Net/Url.h"

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
    static MessageDeque* GetInstance();
    void                 PushWaitedMessage(const std::string& message);
    // [[nodiscard]] std::string PopWaitedMessage();
    void ClearWaitedMessage();
    void AddGiftMessage(const std::string& user, const std::string& giftName, uint32_t giftCount);
    void AddBlindBoxMessage(const std::string& user, const std::string& giftName,
                            uint32_t giftCount, int32_t profitAndLoss);
    // void ClearGiftMessage();

private:
    void SendMessageInThread();
    bool SendMessageToBili(const std::string& message);
    bool SendGiftMessageToBili();
    ///@brief 定时检查答谢延迟标志，为true就发送，为false就设置为true，等待答谢延迟时间重新执行
    bool CheckSendMessageToBili();

private:
    MessageDeque();
    ~MessageDeque() = default;

    MessageDeque(const MessageDeque& other)            = delete;
    MessageDeque(MessageDeque&& other)                 = delete;
    MessageDeque& operator=(const MessageDeque& other) = delete;
    MessageDeque& operator=(MessageDeque&& other)      = delete;

private:
    static MessageDeque* pInstance;

private:
    std::queue<std::string> messageQueue;
    std::mutex              messageQueueMutex;
    std::condition_variable messageQueueCondtionVariable;
    std::thread             sendMessageThread;

private:
    const static Url url;

private:
    boost::asio::io_context        ioc;        // IO上下文
    boost::asio::ssl::context      ctx;        // SSL上下文
    boost::asio::ip::tcp::resolver resolver;   // DNS解析器
private:
    std::mutex giftMapMutex;   // 礼物消息发送的互斥锁
    std::unordered_map<std::string, std::unordered_map<std::string, uint32_t>>
        giftMap;   // 待发送的礼物消息，key1为赠送人，key2为礼物名称，value为礼物数量
    std::unordered_map<std::string, std::unordered_map<std::string, std::pair<uint32_t, double>>>
        blindBoxMap;   // 待发送的礼物消息，key1为赠送人，key2为礼物名称，value为礼物数量
    boost::asio::io_context   sendGiftMessageIoc;     // 发送礼物消息的IO上下文
    boost::asio::steady_timer sendGiftMessageTimer;   // 发送弹幕定时器
    bool                      sendGiftMessageTip;     // 答谢延迟标志
    std::thread               checkGiftThread;
};



#endif   // MESSAGEDEQUE_H
