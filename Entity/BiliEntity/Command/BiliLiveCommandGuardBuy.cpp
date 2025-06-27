//
// Created by zeng on 24-7-7.
//

#include "BiliLiveCommandGuardBuy.h"

#include "../../DataBase/DataBase.h"
#include "../../Global/Config.h"
#include "../../MessageDeque/MessageDeque.h"

std::map<int, std::string> BiliLiveCommandGuardBuy::guardLevelMap{
    {1, "总督"}, {2, "提督"}, {3, "舰长"},
    // {4, "司令"},
    // {5, "元帅"},
    // {6, "大元帅"},
    // {7, "大将军"},
    // {8, "大统领"},
    // {9, "大元帅"},
};
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
    return fmt::format("User: {} 购买了舰长, GuardLevel: {}, Num: {}, Price: {}",
                       user.GetUname(),
                       guardLevel,
                       num,
                       price);
}

void BiliLiveCommandGuardBuy::Run() const
{
    LOG_MESSAGE(LogLevel::Info, this->ToString());
    if (Config::GetInstance()->CanThanksGift())
    {
        MessageDeque::GetInstance()->PushWaitedMessage(fmt::format(
            "感谢{}购买了{}x｛｝", user.GetUname(), this->guardLevelMap[this->guardLevel], num));
    }
    else
    {
        LOG_MESSAGE(LogLevel::Debug, "Guard buy message ignored due to configuration.");
    }
    if (const bool res = DataBase::GetInstance()->AddGift(
            this->guardLevelMap[this->guardLevel], this->price, this->num, this->user);
        !res)
    {
        LOG_MESSAGE(LogLevel::Error,
                    fmt::format("Failed to add Guard Buy to {}", this->ToString()));
    }
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