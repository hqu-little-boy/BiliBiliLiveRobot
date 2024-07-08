//
// Created by zeng on 24-7-8.
//

#include "BiliLiveCommandAnchorLotStart.h"

BiliLiveCommandAnchorLotStart::BiliLiveCommandAnchorLotStart(const nlohmann::json& message)
    : BiliLiveCommandBase(message)
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
    LOG_MESSAGE(LogLevel::INFO, this->ToString());
}