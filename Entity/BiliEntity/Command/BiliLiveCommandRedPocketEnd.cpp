//
// Created by zeng on 24-7-8.
//

#include "BiliLiveCommandRedPocketEnd.h"

BiliLiveCommandRedPocketEnd::BiliLiveCommandRedPocketEnd(const nlohmann::json& message)
    : BiliLiveCommandBase(message)
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
    LOG_MESSAGE(LogLevel::INFO, this->ToString());
}