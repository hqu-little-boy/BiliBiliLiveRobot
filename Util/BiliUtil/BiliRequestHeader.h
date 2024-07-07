//
// Created by zeng on 24-7-2.
//

#ifndef BILIHEADER_H
#define BILIHEADER_H
#include "BiliCookie.h"

#include <string>


/// @brief 单例模式的request header
class BiliRequestHeader
{
public:
    static BiliRequestHeader*        GetInstance();
    static void                      DestroyInstance();
    [[nodiscard]] const std::string& GetUserAgent() const;
    void                             SetUserAgent(const std::string& userAgent);

    const BiliCookie& GetBiliCookie() const;
    bool              LoadBiliCookieByPath(const std::string_view& jsonPath);
    bool              LoadBiliCookieByJson(const nlohmann::json& json);

private:
    BiliRequestHeader();
    ~BiliRequestHeader()                                          = default;
    BiliRequestHeader(const BiliRequestHeader&)                   = delete;
    BiliRequestHeader&        operator=(const BiliRequestHeader&) = delete;
    static BiliRequestHeader* pInstance;
    std::string               userAgent;
    BiliCookie                biliCookie;
};



#endif   // BILIHEADER_H
