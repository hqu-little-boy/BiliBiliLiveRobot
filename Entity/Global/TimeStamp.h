//
// Created by zeng on 24-6-10.
//

#ifndef TIMESTAMP_H
#define TIMESTAMP_H
#include <chrono>
#include <cstdint>
#include <string>


class TimeStamp
{
public:
    TimeStamp();
    explicit TimeStamp(int64_t microSecondsSinceEpoch);
    explicit TimeStamp(std::chrono::system_clock::time_point timePoint);

    [[nodiscard]] static TimeStamp Now();
    [[nodiscard]] std::string      ToString() const;
    [[nodiscard]] bool             IsTimeOut(std::chrono::milliseconds milliseconds) const;
    [[nodiscard]] bool             IsTimeOut(std::chrono::microseconds microseconds) const;
    [[nodiscard]] bool             IsTimeOut(std::chrono::seconds seconds) const;
    [[nodiscard]] bool             IsTimeOut(std::chrono::minutes minutes) const;
    [[nodiscard]] bool             IsTimeOut(std::chrono::hours hours) const;
    [[nodiscard]] bool             IsTimeOut(std::chrono::days days) const;

private:
    std::chrono::system_clock::time_point timePoint;
};



#endif   // TIMESTAMP_H
