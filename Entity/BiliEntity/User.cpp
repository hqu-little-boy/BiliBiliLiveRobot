//
// Created by zeng on 24-7-6.
//

#include "User.h"

#include <format>

User::User(uint64_t uid, const std::string& uname)
    : uid(uid)
    , uname(uname)
    , level(0)
    , fanCount(0)
{
}

// User::User(const User& other)
//     : uid(other.uid)
//     , uname(other.uname)
//     , level(other.level)
//     , fanCount(other.fanCount)
// {
// }
//
// User::User(User&& other)
//     : uid(other.uid)
//     , uname(std::move(other.uname))
//     , level(other.level)
//     , fanCount(other.fanCount)
// {
// }
//
// User& User::operator=(const User& other)
// {
//     if (this == &other)
//     {
//         return *this;
//     }
//     this->uid      = other.uid;
//     this->uname    = other.uname;
//     this->level    = other.level;
//     this->fanCount = other.fanCount;
//     return *this;
// }
//
// User& User::operator=(User&& other) noexcept
// {
//     if (this == &other)
//     {
//         return *this;
//     }
//     this->uid      = other.uid;
//     this->uname    = std::move(other.uname);
//     this->level    = other.level;
//     this->fanCount = other.fanCount;
//     return *this;
// }

uint64_t User::GetUid() const
{
    return this->uid;
}

const std::string& User::GetUname() const
{
    return this->uname;
}

std::string User::ToString() const
{
    return std::format("uid: {}, uname: {}", this->uid, this->uname);
}