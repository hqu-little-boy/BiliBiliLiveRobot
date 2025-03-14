//
// Created by zeng on 24-7-6.
//

#include "BiliLiveCommandBase.h"


BiliLiveCommandBase::BiliLiveCommandBase()
    : timestamp(TimeStamp::Now())
{
}

bool BiliLiveCommandBase::IsTimeOut() const
{
    return this->timestamp.IsTimeOut(std::chrono::minutes(5));
}

void BiliLiveCommandBase::SetTimeStamp()
{
    this->timestamp = TimeStamp::Now();
}

BiliApiUtil::LiveCommand BiliLiveCommandBase::GetCommandType() const
{
    return commandType;
}
