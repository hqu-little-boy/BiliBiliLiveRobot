//
// Created by zeng on 24-7-8.
//

#include "BiliLiveCommandAnchorLotEnd.h"

#include "../../Global/Config.h"
#include "../../MessageDeque/MessageDeque.h"

BiliLiveCommandAnchorLotEnd::BiliLiveCommandAnchorLotEnd()
    : BiliLiveCommandBase()
{
}

std::string BiliLiveCommandAnchorLotEnd::ToString() const
{
    return "天选之人抽奖结束";
}

bool BiliLiveCommandAnchorLotEnd::LoadMessage(const nlohmann::json& message)
{
    return true;
}

void BiliLiveCommandAnchorLotEnd::Run() const
{
    LOG_MESSAGE(LogLevel::Info, this->ToString());

    if (Config::GetInstance()->CanEntryNotice())
    {
        Config::GetInstance()->SetCanEntryNotice(true);
        MessageDeque::GetInstance()->PushWaitedMessage("天选抽奖结束，重新开启欢迎信息。");
    }
    // if (Config::GetInstance()->CanThanksFocus())
    // {
    //     Config::GetInstance()->SetCanThanksFocus(true);
    // }
    // if (Config::GetInstance()->CanThanksShare())
    // {
    //     Config::GetInstance()->SetCanThanksShare(true);
    // }
}