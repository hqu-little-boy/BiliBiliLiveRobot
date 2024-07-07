//
// Created by zeng on 24-7-6.
//

#ifndef USER_H
#define USER_H
#include <cstdint>
#include <string>


class User
{
public:
    User(uint64_t uid, const std::string& uname);
    // User(const User& other);
    // User(User&& other);
    // User& operator=(const User& other);
    // User& operator=(User&& other);
    ~User() = default;
    [[nodiscard]] uint64_t           GetUid() const;
    [[nodiscard]] const std::string& GetUname() const;
    std::string                      ToString() const;

private:
    uint64_t    uid;
    std::string uname;
    unsigned    level;      // 荣耀等级
    unsigned    fanCount;   // 粉丝数
};



#endif   // USER_H
