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
        //使用utfcpp库分割字符串
        std::string::const_iterator start = message.begin();
        std::string::const_iterator end   = message.begin();
        while (end != message.end())
        {
            utf8::next(end, message.end());
            if (utf8::distance(start, end) > 20)
            {
                std::string subMessage{start, end};
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

void MessageDeque::SendMessageInThread()
{
    while (true)
    {
        std::unique_lock<std::mutex> lock{this->messageQueueMutex};
        this->messageQueueCondtionVariable.wait(lock,
                                                [this] { return !this->messageQueue.empty(); });
        std::string message{std::move(this->messageQueue.front())};
        this->messageQueue.pop();
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
    const auto results = resolver.resolve(url.GetHost(), std::to_string(url.GetPort()));

    // 创建SSL流并连接
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket> stream(this->ioc, this->ctx);
    boost::asio::connect(stream.next_layer(), results.begin(), results.end());
    stream.handshake(boost::asio::ssl::stream_base::client);
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
    boost::beast::http::write(stream, req);

    // 获取响应
    boost::beast::flat_buffer                                      buffer;
    boost::beast::http::response<boost::beast::http::dynamic_body> res;
    boost::beast::http::read(stream, buffer, res);
    if (res.result() != boost::beast::http::status::ok)
    {
        return false;
    }
    LOG_VAR(LogLevel::Info, boost::beast::buffers_to_string(res.body().data()));
    return true;
}

MessageDeque::MessageDeque()
    : ioc(1)
    , ctx(boost::asio::ssl::context::tlsv12_client)
    , resolver(this->ioc)
    , snedMessageThread(&MessageDeque::SendMessageInThread, this)
{
}