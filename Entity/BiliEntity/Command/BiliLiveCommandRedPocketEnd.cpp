//
// Created by zeng on 24-7-8.
//

#include "BiliLiveCommandRedPocketEnd.h"

#include "../../Global/Config.h"
#include "../../MessageDeque/MessageDeque.h"

BiliLiveCommandRedPocketEnd::BiliLiveCommandRedPocketEnd()
    : BiliLiveCommandBase()
{
}

std::string BiliLiveCommandRedPocketEnd::ToString() const
{
    return "红包抽奖结束";
}

bool BiliLiveCommandRedPocketEnd::LoadMessage(const nlohmann::json& message)
{
    return true;
}

void BiliLiveCommandRedPocketEnd::Run() const
{
    LOG_MESSAGE(LogLevel::Info, this->ToString());
    if (Config::GetInstance()->CanEntryNotice())
    {
        Config::GetInstance()->SetCanEntryNotice(true);
        MessageDeque::GetInstance()->PushWaitedMessage("红包抽奖结束，重新开启欢迎信息。");
    }
    if (Config::GetInstance()->CanThanksFocus())
    {
        Config::GetInstance()->SetCanThanksFocus(true);
    }
    if (Config::GetInstance()->CanThanksShare())
    {
        Config::GetInstance()->SetCanThanksShare(true);
    }
}