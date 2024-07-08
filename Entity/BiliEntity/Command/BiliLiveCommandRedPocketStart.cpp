//
// Created by zeng on 24-7-8.
//

#include "BiliLiveCommandRedPocketStart.h"

BiliLiveCommandRedPocketStart::BiliLiveCommandRedPocketStart(const nlohmann::json& message)
    : BiliLiveCommandBase(message)
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
    LOG_MESSAGE(LogLevel::INFO, this->ToString());
}