//
// Created by zeng on 24-7-7.
//

#include "BiliLiveCommandInteractWord.h"

#include "../../Global/Config.h"
#include "../../MessageDeque/MessageDeque.h"

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
        return std::format("{}进入直播间", user.ToString());
        break;
    }
    case 2:
    {
        return std::format("{}关注了主播", user.ToString());
        break;
    }
    case 3:
    {
        return std::format("{}分享了直播间", user.ToString());
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
    // MessageDeque::GetInstance()->PushWaitedMessage(std::format("欢迎{}进入直播间",
    // this->user.GetUname()));
    if (this->user.GetUid() == 0)
    {
        return;
    }
    if (this->megType == 1 && Config::GetInstance()->CanEntryNotice())
    {
        try
        {
            std::string message{std::vformat(Config::GetInstance()->GetNormalEntryNoticeWord(),
                                             std::make_format_args(this->user.GetUname()))};
            MessageDeque::GetInstance()->PushWaitedMessage(message);
            LOG_VAR(LogLevel::Debug, message);
        }
        catch (const std::exception& e)
        {
            LOG_MESSAGE(LogLevel::Error, e.what());
        }
    }
    else if (this->megType == 2 && Config::GetInstance()->CanThanksFocus())
    {
        // MessageDeque::GetInstance()->PushWaitedMessage(
        //     std::format("感谢{}关注了主播", this->user.GetUname()));
        std::string message{std::format("感谢{}关注了主播", this->user.GetUname())};
        MessageDeque::GetInstance()->PushWaitedMessage(message);
        LOG_VAR(LogLevel::Debug, message);
    }
    else if (this->megType == 3 && Config::GetInstance()->CanThanksShare())
    {
        // MessageDeque::GetInstance()->PushWaitedMessage(
        //     std::format("感谢{}分享了直播间", this->user.GetUname()));
        std::string message{std::format("感谢{}分享了直播间", this->user.GetUname())};
        MessageDeque::GetInstance()->PushWaitedMessage(message);
        LOG_VAR(LogLevel::Debug, message);
    }
}

// BiliApiUtil::LiveCommand BiliLiveCommandInteractWord::GetCommandType() const
// {
//     return commandType;
// }