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
        if (!this->SendMessage(message))
        {
            LOG_MESSAGE(LogLevel::Warn, "Send message failed");
        }
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

bool MessageDeque::SendMessage(const std::string& message)
{
    return true;
}

MessageDeque::MessageDeque()
    : ioc(1)
    , ctx(boost::asio::ssl::context::tlsv12_client)
    , resolver(this->ioc)
{
}