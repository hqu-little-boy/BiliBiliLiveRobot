//
// Created by zeng on 24-7-2.
//

#ifndef BILILOGIN_H
#define BILILOGIN_H
#include <string>


class BiliLogin
{
public:
    static bool GetLoginQRCode();
    static bool GetLoginInfo();

    constexpr static std::string_view qrCodeUrl{
        "https://passport.bilibili.com/x/passport-login/web/qrcode/generate"};
    constexpr static std::string_view loginInfoUrl{
        "https://passport.bilibili.com/x/passport-login/web/qrcode/poll"};
};



#endif   // BILILOGIN_H
