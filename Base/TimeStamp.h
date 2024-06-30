//
// Created by zeng on 24-6-10.
//

#ifndef TIMESTAMP_H
#define TIMESTAMP_H
#include <cstdint>
#include <string>


class TimeStamp {
public:
    TimeStamp();
    explicit TimeStamp(int64_t microSecondsSinceEpoch);
    static TimeStamp Now();
    [[nodiscard]] std::string ToString() const;
private:
    int64_t microSecondsSinceEpoch;
};



#endif //TIMESTAMP_H
