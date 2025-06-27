//
// Created by zeng on 24-7-6.
//

#include "MessageDeque.h"

#include "../../Util/BiliUtil/BiliRequestHeader.h"
#include "../Global/Config.h"
#include "../Net/Multipart.h"
#include "utf8.h"

#include <boost/asio/connect.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/beast/core/buffers_to_string.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/http/dynamic_body.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/read.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/http/write.hpp>

const Url     MessageDeque::url{"api.live.bilibili.com", 443, "/msg/send"};
MessageDeque* MessageDeque::pInstance{new MessageDeque()};
MessageDeque* MessageDeque::GetInstance()
{
    return pInstance;
}

void MessageDeque::PushWaitedMessage(const std::string& message)
{
    int length = utf8::distance(message.begin(), message.end());
    if (length > 40)
    {
        LOG_VAR(LogLevel::Warn, message);
        LOG_MESSAGE(LogLevel::Warn, "Message is too long");
        return;
    }
    else if (length < 1)
    {
        LOG_VAR(LogLevel::Warn, message);
        LOG_MESSAGE(LogLevel::Warn, "Message is too short");
        return;
    }
    std::unique_lock<std::mutex> lock{this->messageQueueMutex};
    if (length > 20)
    {
        // 使用utfcpp库分割字符串
        std::string::const_iterator start = message.begin();
        std::string::const_iterator end   = message.begin();
        while (end != message.end())
        {
            utf8::next(end, message.end());
            if (utf8::distance(start, end) > 19 || end == message.end())
            {
                std::string subMessage{start, end};
                // LOG_VAR(LogLevel::Test, subMessage);
                this->messageQueue.push(subMessage);
                start = end;
            }
        }
    }
    else
    {
        this->messageQueue.push(message);
    }
    this->messageQueueCondtionVariable.notify_one();
}

// std::string MessageDeque::PopWaitedMessage()
// {
//     std::unique_lock<std::mutex> lock{this->messageQueueMutex};
//     this->messageQueueCondtionVariable.wait(lock, [this] { return !this->messageQueue.empty();
//     }); std::string message{std::move(this->messageQueue.front())}; this->messageQueue.pop();
//     return message;
// }

void MessageDeque::ClearWaitedMessage()
{
    std::unique_lock<std::mutex> lock{this->messageQueueMutex};
    while (!this->messageQueue.empty())
    {
        this->messageQueue.pop();
    }
}
void MessageDeque::AddGiftMessage(const std::string& user,
                                  const std::string& giftName,
                                  uint32_t           giftCount)
{
    std::unique_lock<std::mutex> lock{this->giftMapMutex};
    auto&                        giftMap{this->giftMap[user]};
    if (giftMap.contains(giftName))
    {
        giftMap[giftName] += giftCount;
    }
    else
    {
        giftMap[giftName] = giftCount;
    }
    this->sendGiftMessageTip = false;   // 设置发送弹幕标志
}
void MessageDeque::AddBlindBoxMessage(const std::string& user,
                                      const std::string& giftName,
                                      uint32_t           giftCount,
                                      int32_t            profitAndLoss)
{
    std::unique_lock<std::mutex> lock{this->giftMapMutex};
    auto&                        blindBoxMap{this->blindBoxMap[user]};
    if (blindBoxMap.contains(giftName))
    {
        blindBoxMap[giftName].first += giftCount;
        blindBoxMap[giftName].second += static_cast<double>(profitAndLoss) / 1000;
    }
    else
    {
        blindBoxMap[giftName] = {giftCount, static_cast<double>(profitAndLoss) / 1000};
    }
    this->sendGiftMessageTip = false;
}
// void MessageDeque::ClearGiftMessage()
// {
//     std::unique_lock<std::mutex> lock{this->giftMapMutex};
//     this->giftMap.clear();
// }

void MessageDeque::SendMessageInThread()
{
    while (true)
    {
        std::unique_lock<std::mutex> lock{this->messageQueueMutex};
        this->messageQueueCondtionVariable.wait(lock,
                                                [this] { return !this->messageQueue.empty(); });
        std::string message{std::move(this->messageQueue.front())};
        this->messageQueue.pop();
#if defined(TEST)
        if (Config::GetInstance()->IsTest())
        {
            LOG_VAR(LogLevel::Test, message);
            continue;
        }
#endif
        if (!this->SendMessageToBili(message))
        {
            LOG_MESSAGE(LogLevel::Warn, "Send message failed");
        }
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

bool MessageDeque::SendMessageToBili(const std::string& message)
{
    LOG_VAR(LogLevel::Debug, url.ToString());
    // 解析域名和端口
    boost::system::error_code ecResolver;
    const auto results = resolver.resolve(url.GetHost(), std::to_string(url.GetPort()), ecResolver);
    if (ecResolver)
    {
        LOG_VAR(LogLevel::Error,
                fmt::format("Failed to resolve {}: {}", url.GetHost(), ecResolver.message()));
        return false;
    }
    // 创建SSL流并连接
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket> stream(this->ioc, this->ctx);
    boost::system::error_code                              ecConnect;
    boost::asio::connect(stream.next_layer(), results.begin(), results.end(), ecConnect);
    if (ecConnect)
    {
        LOG_VAR(LogLevel::Error, fmt::format("Failed to connect: {}", ecConnect.message()));
        return false;
    }
    // 执行SSL握手
    boost::system::error_code ecHandshake;
    stream.handshake(boost::asio::ssl::stream_base::client, ecHandshake);
    if (ecHandshake)
    {
        LOG_VAR(LogLevel::Error,
                fmt::format("Failed to perform SSL handshake: {}", ecHandshake.message()));
        return false;
    }
    Multipart multipart{
        {"bubble", "5"},
        {"msg", message},
        {"color", "4546550"},
        // {"mode", "1"},
        {"room_type", "0"},
        // {"jumpfrom", "71002"},
        // {"reply_mid", "0"},
        // {"reply_attr", "0"},
        // {"replay_dmid", ""},
        // {"statistics", "{\"appId\":100,\"platform\":5}"},
        {"fontsize", "25"},
        {"rnd", std::to_string(std::time(nullptr))},
        {"roomid", std::to_string(Config::GetInstance()->GetRoomId())},
        {"csrf", BiliRequestHeader::GetInstance()->GetBiliCookie().GetBiliJct()},
        {"csrf_token", BiliRequestHeader::GetInstance()->GetBiliCookie().GetBiliJct()}};
    // 构建POST请求
    boost::beast::http::request<boost::beast::http::string_body> req{
        boost::beast::http::verb::post, url.GetTarget(), 11};
    req.set(boost::beast::http::field::host, url.GetHost());
    req.set(boost::beast::http::field::user_agent,
            BiliRequestHeader::GetInstance()->GetUserAgent());
    req.set(boost::beast::http::field::content_type,
            multipart.GetSerializeMultipartFormdataGetContentType());
    // 设置cookie
    req.set(boost::beast::http::field::cookie,
            BiliRequestHeader::GetInstance()->GetBiliCookie().ToString());
    // 设置请求体
    req.body() = multipart.GetSerializeMultipartFormdata();
    req.prepare_payload();
    // 发送请求
    boost::system::error_code ecSend;
    boost::beast::http::write(stream, req, ecSend);
    if (ecSend)
    {
        LOG_VAR(LogLevel::Error, fmt::format("Failed to send request: {}", ecSend.message()));
        return false;
    }
    // 获取响应
    boost::beast::flat_buffer                                      buffer;
    boost::beast::http::response<boost::beast::http::dynamic_body> res;
    boost::system::error_code                                      ecRead;
    boost::beast::http::read(stream, buffer, res, ecRead);
    if (ecRead)
    {
        LOG_VAR(LogLevel::Error, fmt::format("Failed to read response: {}", ecRead.message()));
        return false;
    }
    if (res.result() != boost::beast::http::status::ok)
    {
        return false;
    }
    // LOG_VAR(LogLevel::Info, boost::beast::buffers_to_string(res.body().data()));
    std::string    resStr       = boost::beast::buffers_to_string(res.body().data());
    nlohmann::json roomInfoJson = nlohmann::json::parse(resStr);
    LOG_VAR(LogLevel::Debug, roomInfoJson.dump(-1));
    try
    {
        if (roomInfoJson["code"] != 0)
        {
            LOG_VAR(LogLevel::Error, roomInfoJson["message"].dump(4));
            return false;
        }
    }
    catch (const nlohmann::json::exception& e)
    {
        LOG_VAR(LogLevel::Error, e.what());
        return false;
    }
    return true;
}
bool MessageDeque::SendGiftMessageToBili()
{
    std::unique_lock<std::mutex> lock{this->giftMapMutex};
    if (this->giftMap.empty() || this->blindBoxMap.empty())
    {
        LOG_MESSAGE(LogLevel::Debug, "No gift message to send");
        return true;   // 没有礼物消息，直接返回
    }
    for (const auto& [user, gifts] : this->giftMap)
    {
        std::string giftMessage;
        for (const auto& [giftName, giftCount] : gifts)
        {
            if (!giftMessage.empty())
            {
                giftMessage += "、";
            }
            giftMessage += fmt::format("{}x{}", giftName, giftCount);
        }
        this->PushWaitedMessage(fmt::format("感谢{}的礼物: {}", user, giftMessage));
    }
    for (const auto& [user, blindBoxs] : this->blindBoxMap)
    {
        std::string blindBoxMessage;
        for (const auto& [giftName, giftInfo] : blindBoxs)
        {
            if (!blindBoxMessage.empty())
            {
                blindBoxMessage += "、";
            }
            if (giftInfo.second > 0)
            {
                blindBoxMessage +=
                    fmt::format("{}x{}(赚{:.2f}元)", giftName, giftInfo.first, giftInfo.second);
            }
            else if (giftInfo.second < 0)
            {
                blindBoxMessage +=
                    fmt::format("{}x{}(亏{:.2f}元)", giftName, giftInfo.first, -giftInfo.second);
            }
            else
            {
                blindBoxMessage += fmt::format("{}x{}(不亏，不赚)", giftName, giftInfo.first);
            }
        }
        this->PushWaitedMessage(fmt::format("感谢{}的盲盒: {}", user, blindBoxMessage));
    }
    this->giftMap.clear();
    this->blindBoxMap.clear();
    this->sendGiftMessageTip = false;
    return true;
}
bool MessageDeque::CheckSendMessageToBili()
{
    if (this->sendGiftMessageTip)
    {
        this->SendGiftMessageToBili();
    }
    else
    {
        std::unique_lock<std::mutex> lock{this->messageQueueMutex};
        if (!this->giftMap.empty())
        {
            this->sendGiftMessageTip = true;
        }
    }
    this->sendGiftMessageTimer.expires_after(
        std::chrono::seconds(Config::GetInstance()->GetThanksGiftTimeout()));
    this->sendGiftMessageTimer.async_wait([this](const boost::system::error_code& ec) {
        if (ec)
        {
            LOG_VAR(LogLevel::Error, fmt::format("sendGiftMessageTimer error: {}", ec.message()));
            return;
        }
        this->CheckSendMessageToBili();
    });
    return true;
}

MessageDeque::MessageDeque()
    : sendMessageThread(&MessageDeque::SendMessageInThread, this)
    , ioc(1)
    , ctx(boost::asio::ssl::context::tlsv12_client)
    , resolver(this->ioc)
    , sendGiftMessageIoc{}
    , sendGiftMessageTimer(this->sendGiftMessageIoc)
    , sendGiftMessageTip(false)
{
    this->sendGiftMessageTimer.expires_after(
        std::chrono::seconds(Config::GetInstance()->GetThanksGiftTimeout()));
    this->sendGiftMessageTimer.async_wait([this](const boost::system::error_code& ec) {
        if (ec)
        {
            LOG_VAR(LogLevel::Error, fmt::format("sendGiftMessageTimer error: {}", ec.message()));
            return;
        }
        this->CheckSendMessageToBili();
    });
    this->checkGiftThread = std::thread([this] {
        LOG_MESSAGE(LogLevel::Info, "Check gift thread started");
        this->sendGiftMessageIoc.run();
    });
}