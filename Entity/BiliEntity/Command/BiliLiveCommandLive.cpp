//
// Created by zeng on 24-7-8.
//

#include "BiliLiveCommandLive.h"

BiliLiveCommandLive::BiliLiveCommandLive(const nlohmann::json& message)
    : BiliLiveCommandBase(message)
{
}

std::string BiliLiveCommandLive::ToString() const
{
    return "开播啦";
}

bool BiliLiveCommandLive::LoadMessage(const nlohmann::json& message)
{
    return true;
}

void BiliLiveCommandLive::Run() const
{
    LOG_MESSAGE(LogLevel::Info, "开播啦");
}