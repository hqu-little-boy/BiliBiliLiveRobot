//
// Created by zeng on 24-7-7.
//

#include "BiliLiveCommandInteractWord.h"

BiliLiveCommandInteractWord::BiliLiveCommandInteractWord(const nlohmann::json& message)
    : BiliLiveCommandBase{message}
    , megType{UINT_MAX}
    , user{0, "", 0}
{
    LoadMessage(message);
}

std::string BiliLiveCommandInteractWord::ToString() const
{
    switch (megType)
    {
    case 1:
    {
        return std::format("{}进入直播间", user.GetUname());
        break;
    }
    case 2:
    {
        return std::format("{}关注了主播", user.GetUname());
        break;
    }
    case 3:
    {
        return std::format("{}分享了直播间", user.GetUname());
        break;
    }
    default:
    {
        LOG_VAR(LogLevel::Error, megType);
        break;
    }
    }
    return "";
}

bool BiliLiveCommandInteractWord::LoadMessage(const nlohmann::json& message)
{
    try
    {
        const auto& data{message["data"]};
        const auto& uinfo{message["data"]["uinfo"]};
        this->megType = data["msg_type"].get<unsigned>();
        // LOG_VAR(LogLevel::Debug, megType);
        auto uid{data["uid"].get<uint64_t>()};
        // LOG_VAR(LogLevel::Debug, uid);
        auto uname{uinfo["base"]["name"].get<std::string>()};
        // LOG_VAR(LogLevel::Debug, uname);
        auto guardLevel{0u};
        if (uinfo.contains("medal") && uinfo["medal"].contains("guard_level"))
        {
            guardLevel = uinfo["medal"]["guard_level"].get<unsigned>();
        }
        // LOG_VAR(LogLevel::Debug, guardLevel);
        auto wealthLevel{0u};
        if (uinfo.contains("wealth") && uinfo["wealth"].contains("level"))
        {
            wealthLevel = uinfo["wealth"]["level"].get<unsigned>();
        }
        // LOG_VAR(LogLevel::Debug, wealthLevel);
        this->user = User(uid, uname, guardLevel, wealthLevel);
        return true;
    }
    catch (const nlohmann::json::exception& e)
    {
        LOG_MESSAGE(LogLevel::Error, e.what());
        return false;
    }
}

void BiliLiveCommandInteractWord::Run() const
{
    LOG_MESSAGE(LogLevel::Info, this->ToString());
}

// BiliApiUtil::LiveCommand BiliLiveCommandInteractWord::GetCommandType() const
// {
//     return commandType;
// }