//
// Created by zeng on 24-6-10.
//

#include "TimeStamp.h"

#include <chrono>

TimeStamp::TimeStamp()
    : microSecondsSinceEpoch(0)
{
}

TimeStamp::TimeStamp(int64_t microSecondsSinceEpoch)
    : microSecondsSinceEpoch(microSecondsSinceEpoch)
{
}

TimeStamp TimeStamp::Now()
{
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    return TimeStamp(now_time_t);
}

std::string TimeStamp::ToString() const
{
    std::tm now_tm = *std::localtime(&microSecondsSinceEpoch);
    // 使用 std::put_time 将时间格式化为字符串
    std::stringstream ss;
    ss << std::put_time(&now_tm, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}
