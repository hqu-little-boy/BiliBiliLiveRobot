//
// Created by zeng on 24-7-8.
//

#include "BiliLiveCommandPreparing.h"

BiliLiveCommandPreparing::BiliLiveCommandPreparing(const nlohmann::json& message)
    : BiliLiveCommandBase(message)
{
}

std::string BiliLiveCommandPreparing::ToString() const
{
    return "下播啦";
}

bool BiliLiveCommandPreparing::LoadMessage(const nlohmann::json& message)
{
    return true;
}

void BiliLiveCommandPreparing::Run() const
{
    LOG_MESSAGE(LogLevel::INFO, "下播啦");
}
