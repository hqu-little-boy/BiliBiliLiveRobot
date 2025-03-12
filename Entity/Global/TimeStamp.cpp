//
// Created by zeng on 24-6-10.
//

#include "TimeStamp.h"

#include <fmt/chrono.h>
#include <fmt/format.h>
#include <iomanip>

TimeStamp::TimeStamp()
    : timePoint{std::chrono::system_clock::now()}
{
}

TimeStamp::TimeStamp(int64_t microSecondsSinceEpoch)
    : timePoint(std::chrono::system_clock::from_time_t(microSecondsSinceEpoch))
{
}
TimeStamp::TimeStamp(std::chrono::system_clock::time_point timePoint)
    : timePoint(timePoint)
{
}



TimeStamp TimeStamp::Now()
{
    return TimeStamp();
}

std::string TimeStamp::ToString() const
{
    // std::tm now_tm = *std::localtime(&microSecondsSinceEpoch);
    // // 使用 std::put_time 将时间格式化为字符串
    // std::stringstream ss;
    // ss << std::put_time(&now_tm, "%Y-%m-%d %H:%M:%S
    // return ss.str();
    // std::time_t       tt = std::chrono::system_clock::to_time_t(timePoint);
    // std::tm           tm = *std::gmtime(&tt);
    // std::stringstream ss;
    // ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    // return ss.str();
    return fmt::format("{:%Y-%m-%d %H:%M:%S}", std::chrono::current_zone()->to_local(timePoint));
}

bool TimeStamp::IsTimeOut(std::chrono::milliseconds milliseconds) const
{
    auto now = std::chrono::system_clock::now();
    return now - timePoint > milliseconds;
}

bool TimeStamp::IsTimeOut(std::chrono::microseconds microseconds) const
{
    auto now = std::chrono::system_clock::now();
    return now - timePoint > microseconds;
}

bool TimeStamp::IsTimeOut(std::chrono::seconds seconds) const
{
    auto now = std::chrono::system_clock::now();
    return now - timePoint > seconds;
}

bool TimeStamp::IsTimeOut(std::chrono::minutes minutes) const
{
    auto now = std::chrono::system_clock::now();
    return now - timePoint > minutes;
}

bool TimeStamp::IsTimeOut(std::chrono::hours hours) const
{
    auto now = std::chrono::system_clock::now();
    return now - timePoint > hours;
}

bool TimeStamp::IsTimeOut(std::chrono::days days) const
{
    auto now = std::chrono::system_clock::now();
    return now - timePoint > days;
}
