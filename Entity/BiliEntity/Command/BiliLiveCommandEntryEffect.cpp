//
// Created by zeng on 24-7-7.
//

#include "BiliLiveCommandEntryEffect.h"

#include "../../Global/Logger.h"

BiliLiveCommandEntryEffect::BiliLiveCommandEntryEffect()
    : BiliLiveCommandBase()
    , user(0, "", 0)
{
}

std::string BiliLiveCommandEntryEffect::ToString() const
{
    return FORMAT("User: {0}进入直播间", user.GetUname());
}

bool BiliLiveCommandEntryEffect::LoadMessage(const nlohmann::json &message)
{
    try
    {
        const auto &uinfo{message["data"]["uinfo"]};
        auto        uid{uinfo["uid"].get<uint64_t>()};
        auto        uname{uinfo["base"]["name"].get<std::string>()};
        auto        guardLevel = 0;
        if (uinfo.contains("medal") && uinfo["medal"].contains("guard_level"))
        {
            guardLevel = uinfo["medal"]["guard_level"].get<unsigned>();
        }
        auto wealthLevel{uinfo["wealth"]["level"].get<unsigned>()};
        this->user = User(uid, uname, guardLevel, wealthLevel);
        return true;
    }
    catch (const nlohmann::json::exception &e)
    {
        LOG_MESSAGE(LogLevel::Error, e.what());
        return false;
    }
}

void BiliLiveCommandEntryEffect::Run() const
{
    LOG_MESSAGE(LogLevel::Info, this->ToString());
}

// BiliApiUtil::LiveCommand BiliLiveCommandEntryEffect::GetCommandType() const
// {
//     return commandType;
// }
