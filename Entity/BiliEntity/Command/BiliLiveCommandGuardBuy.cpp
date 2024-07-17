//
// Created by zeng on 24-7-7.
//

#include "BiliLiveCommandGuardBuy.h"

BiliLiveCommandGuardBuy::BiliLiveCommandGuardBuy()
    : BiliLiveCommandBase()
    , user(0, "", 0)
    , guardLevel(0)
    , num(0)
    , price(0)
{
}

std::string BiliLiveCommandGuardBuy::ToString() const
{
    return std::format("User: {} 购买了舰长, GuardLevel: {}, Num: {}, Price: {}",
                       user.GetUname(),
                       guardLevel,
                       num,
                       price);
}

void BiliLiveCommandGuardBuy::Run() const
{
    LOG_MESSAGE(LogLevel::Info, this->ToString());
}

bool BiliLiveCommandGuardBuy::LoadMessage(const nlohmann::json& message)
{
    try
    {
        const auto& data{message["data"]};
        auto        uname{data["username"].get<std::string>()};
        auto        uid{data["uid"].get<uint64_t>()};
        auto        guardLevel{data["guard_level"].get<unsigned>()};
        this->user       = User(uid, uname, guardLevel);
        this->guardLevel = data["guard_level"].get<unsigned>();
        this->num        = data["num"].get<unsigned>();
        this->price      = data["price"].get<unsigned>();
    }
    catch (const nlohmann::json::exception& e)
    {
        LOG_MESSAGE(LogLevel::Error, e.what());
        return false;
    }
    return true;
}

// BiliApiUtil::LiveCommand BiliLiveCommandGuardBuy::GetCommandType() const
// {
//     return commandType;
// }