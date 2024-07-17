//
// Created by zeng on 24-7-6.
//

#include "User.h"

#include "../Global/Logger.h"

#include <format>

// User::User(uint64_t uid, const std::string& uname, unsigned guardLevel)
//     : uid(uid)
//     , uname(uname)
//     , level(0)
//     , guardLevel(guardLevel)
// {
// }
//
User::User(uint64_t uid, const std::string& uname, unsigned guardLevel, unsigned level,
           unsigned fanCount, unsigned guardCount)
    : uid(uid)
    , uname(uname)
    , guardLevel(guardLevel)
    , level(level)
    , fanCount(fanCount)
    , guardCount(guardCount)
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
    // return std::format(
    //     "uid: {}, uname: {}, guardLevel: {}", this->uid, this->uname, this->guardLevel);
    return std::format(
        "uid: {}, uname: {}, guardLevel: {}, level: {}, fanCount: {}, guardCount: {}",
        this->uid,
        this->uname,
        this->guardLevel,
        this->level,
        this->fanCount,
        this->guardCount);
}

void User::SetUid(uint64_t uid)
{
    this->uid = uid;
}

void User::SetUname(const std::string& uname)
{
    this->uname = uname;
}

unsigned User::GetFanCount() const
{
    return this->fanCount;
}

void User::SetFanCount(unsigned fanCount)
{
    this->fanCount = fanCount;
}

unsigned User::GetGuardCount() const
{
    return this->guardCount;
}

void User::SetGuardCount(unsigned guardCount)
{
    this->guardCount = guardCount;
}

const std::string User::GetGuardLevel() const
{
    switch (this->guardLevel)
    {
    case 0:
    {
        return "";
    }
    case 1:
    {
        return "总督";
    }
    case 2:
    {
        return "提督";
    }
    case 3:
    {
        return "舰长";
    }
    default:
    {
        LOG_VAR(LogLevel::Error, this->guardLevel);
        return "";
    }
    }
    return "";
}

bool User::IsGuard() const
{
    return this->guardLevel > 0;
}
