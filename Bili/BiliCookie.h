//
// Created by zeng on 24-7-2.
//

#ifndef BILICOOKIE_H
#define BILICOOKIE_H
#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>


class BiliCookie
{
public:
    BiliCookie()  = default;
    ~BiliCookie() = default;
    bool LoadBiliCookieByJson(const nlohmann::json& json);
    // bool                             LoadBiliCookieByPath(const std::string& jsonPath);
    [[nodiscard]] const std::string& GetBuvid3() const;
    [[nodiscard]] const std::string& GetBuvid4() const;
    [[nodiscard]] const std::string& GetSESSDATA() const;
    [[nodiscard]] const std::string& GetBiliJct() const;
    [[nodiscard]] const std::string& GetSid() const;
    [[nodiscard]] uint64_t           GetDedeUserID() const;
    [[nodiscard]] const std::string& GetDedeUserIDCkMd5() const;
    std::string                      ToString() const;

private:
    std::string buvid3;
    std::string buvid4;
    std::string strSESSDATA;
    std::string bili_jct;
    std::string sid;
    uint64_t    strDedeUserID;
    std::string strDedeUserID__ckMd5;
};



#endif   // BILICOOKIE_H
