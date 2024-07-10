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
    // User(uint64_t uid, const std::string& uname, unsigned guardLevel);
    User(uint64_t uid, const std::string& uname, unsigned guardLevel = 0, unsigned level = 0,
         unsigned fanCount = 0, unsigned guardCount = 0);
    // User(const User& other);
    // User(User&& other);
    // User& operator=(const User& other);
    // User& operator=(User&& other);
    ~User() = default;
    [[nodiscard]] uint64_t           GetUid() const;
    void                             SetUid(uint64_t uid);
    [[nodiscard]] const std::string& GetUname() const;
    void                             SetUname(const std::string& uname);
    [[nodiscard]] unsigned           GetFanCount() const;
    void                             SetFanCount(unsigned fanCount);
    [[nodiscard]] unsigned           GetGuardCount() const;
    void                             SetGuardCount(unsigned guardCount);

    [[nodiscard]] std::string ToString() const;

private:
    uint64_t    uid;
    std::string uname;
    unsigned    level;        // 荣耀等级
    unsigned    guardLevel;   // 舰长等级
    unsigned    fanCount;     // 粉丝数
    unsigned    guardCount;   // 舰长数量
};



#endif   // USER_H
