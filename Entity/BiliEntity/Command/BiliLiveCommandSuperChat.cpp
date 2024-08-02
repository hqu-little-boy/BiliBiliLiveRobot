//
// Created by zeng on 24-7-8.
//

#include "BiliLiveCommandSuperChat.h"

BiliLiveCommandSuperChat::BiliLiveCommandSuperChat()
    :BiliLiveCommandBase()
    ,user(0,"",0)
    ,price(0)
    ,message("")
{
}

std::string BiliLiveCommandSuperChat::ToString() const
{
    return FORMAT("User: {} 送出了超级聊天, Price: {}元, Message: {}",
                  user.GetUname(),
                  price,
                  message);
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
}