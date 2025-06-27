//
// Created by zeng on 24-7-8.
//

#include "BiliLiveCommandSuperChat.h"

#include "../../DataBase/DataBase.h"
#include "../../Global/Config.h"
#include "../../MessageDeque/MessageDeque.h"

BiliLiveCommandSuperChat::BiliLiveCommandSuperChat()
    : BiliLiveCommandBase()
    , user(0, "", 0)
    , price(0)
    , message("")
{
}

std::string BiliLiveCommandSuperChat::ToString() const
{
    return fmt::format(
        "User: {} 送出了超级聊天, Price: {}元, Message: {}", user.GetUname(), price, message);
}

bool BiliLiveCommandSuperChat::LoadMessage(const nlohmann::json& message)
{
    try
    {
        const auto& data{message["data"]};
        auto        uname{data["user_info"]["uname"].get<std::string>()};
        auto        uid{data["uid"].get<uint64_t>()};
        this->user    = User(uid, uname, 0);
        this->price   = data["price"].get<unsigned>();
        this->message = data["message"].get<std::string>();
    }
    catch (const nlohmann::json::exception& e)
    {
        LOG_MESSAGE(LogLevel::Error, e.what());
        return false;
    }
    return true;
}

void BiliLiveCommandSuperChat::Run() const
{
    LOG_MESSAGE(LogLevel::Info, this->ToString());
    if (!Config::GetInstance()->CanThanksGift())
    {
        return;
    }
    MessageDeque::GetInstance()->PushWaitedMessage(
        fmt::format("感谢{}送出了{}元的SC:{}", this->user.GetUname(), this->price, this->message));
    if (const bool res = DataBase::GetInstance()->AddGift("SC", this->price, 1, this->user); !res)
    {
        LOG_MESSAGE(LogLevel::Error,
                    fmt::format("Failed to add SuperChat to {}", this->ToString()));
    }
}