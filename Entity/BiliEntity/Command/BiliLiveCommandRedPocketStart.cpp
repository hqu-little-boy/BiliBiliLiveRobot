//
// Created by zeng on 24-7-8.
//

#include "BiliLiveCommandRedPocketStart.h"

#include "../../Global/Config.h"
#include "../../MessageDeque/MessageDeque.h"

BiliLiveCommandRedPocketStart::BiliLiveCommandRedPocketStart()
    : BiliLiveCommandBase()
{
}

std::string BiliLiveCommandRedPocketStart::ToString() const
{
    return "红包雨开始";
}

bool BiliLiveCommandRedPocketStart::LoadMessage(const nlohmann::json& message)
{
    return true;
}

void BiliLiveCommandRedPocketStart::Run() const
{
    LOG_MESSAGE(LogLevel::Info, this->ToString());
    if (Config::GetInstance()->CanEntryNotice())
    {
        Config::GetInstance()->SetCanEntryNotice(false);
        MessageDeque::GetInstance()->PushWaitedMessage("红包抽奖开始，临时关闭欢迎信息。");
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