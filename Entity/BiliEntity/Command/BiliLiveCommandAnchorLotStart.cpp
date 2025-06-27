//
// Created by zeng on 24-7-8.
//

#include "BiliLiveCommandAnchorLotStart.h"

#include "../../Global/Config.h"
#include "../../MessageDeque/MessageDeque.h"

BiliLiveCommandAnchorLotStart::BiliLiveCommandAnchorLotStart()
    : BiliLiveCommandBase()
{
}

std::string BiliLiveCommandAnchorLotStart::ToString() const
{
    return "天选之人抽奖开始";
}

bool BiliLiveCommandAnchorLotStart::LoadMessage(const nlohmann::json& message)
{
    return true;
}

void BiliLiveCommandAnchorLotStart::Run() const
{
    LOG_MESSAGE(LogLevel::Info, this->ToString());
    if (Config::GetInstance()->CanEntryNotice())
    {
        Config::GetInstance()->SetCanEntryNotice(false);
        MessageDeque::GetInstance()->PushWaitedMessage("天选抽奖开始，临时关闭欢迎信息。");
    }
    // if (Config::GetInstance()->CanThanksFocus())
    // {
    //     Config::GetInstance()->SetCanThanksFocus(false);
    // }
    // if (Config::GetInstance()->CanThanksShare())
    // {
    //     Config::GetInstance()->SetCanThanksShare(false);
    // }
}