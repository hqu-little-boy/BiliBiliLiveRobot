//
// Created by zeng on 24-7-7.
//

#include "BiliLiveCommandInteractWord.h"

#include "../../Global/Config.h"
#include "../../MessageDeque/MessageDeque.h"

BiliLiveCommandInteractWord::BiliLiveCommandInteractWord()
    : BiliLiveCommandBase{}
    , megType{UINT_MAX}
    , user{0, "", 0}
{
}

std::string BiliLiveCommandInteractWord::ToString() const
{
    switch (megType)
    {
    case 1:
    {
        return FORMAT("{}进入直播间", user.ToString());
        break;
    }
    case 2:
    {
        return FORMAT("{}关注了主播", user.ToString());
        break;
    }
    case 3:
    {
        return FORMAT("{}分享了直播间", user.ToString());
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
    // MessageDeque::GetInstance()->PushWaitedMessage(FORMAT("欢迎{}进入直播间",
    // this->user.GetUname()));
    if (this->user.GetUid() == 0)
    {
        return;
    }
    if (this->megType == 1 && Config::GetInstance()->CanEntryNotice())
    {
        try
        {
            std::string message{};
            if (!this->user.IsGuard())
            {
                message = VFORMAT(Config::GetInstance()->GetNormalEntryNoticeWord(),
                                  MK_FMT_ARGS(this->user.GetUname()));
            }
            else
            {
                message = VFORMAT(Config::GetInstance()->GetGuardEntryNoticeWord(),
                                  MK_FMT_ARGS(this->user.GetGuardLevel(), this->user.GetUname()));
            }
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
        //     FORMAT("感谢{}关注了主播", this->user.GetUname()));
        std::string message{FORMAT("感谢{}关注了主播", this->user.GetUname())};
        MessageDeque::GetInstance()->PushWaitedMessage(message);
        LOG_VAR(LogLevel::Debug, message);
    }
    else if (this->megType == 3 && Config::GetInstance()->CanThanksShare())
    {
        // MessageDeque::GetInstance()->PushWaitedMessage(
        //     FORMAT("感谢{}分享了直播间", this->user.GetUname()));
        std::string message{FORMAT("感谢{}分享了直播间", this->user.GetUname())};
        MessageDeque::GetInstance()->PushWaitedMessage(message);
        LOG_VAR(LogLevel::Debug, message);
    }
}

// BiliApiUtil::LiveCommand BiliLiveCommandInteractWord::GetCommandType() const
// {
//     return commandType;
// }
