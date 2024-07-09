//
// Created by zeng on 24-7-8.
//

#include "BiliLiveCommandAnchorLotEnd.h"

BiliLiveCommandAnchorLotEnd::BiliLiveCommandAnchorLotEnd(const nlohmann::json& message)
    : BiliLiveCommandBase(message)
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
}