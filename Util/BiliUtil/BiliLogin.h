//
// Created by zeng on 24-7-2.
//

#ifndef BILILOGIN_H
#define BILILOGIN_H
#include "../../Base/noncopyable.h"
#include "../../Entity/Net/Url.h"
#include "qrencode.h"

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/context.hpp>
#include <string>


class BiliLogin : noncopyable
{
public:
    BiliLogin();
    ~BiliLogin() = default;
    std::optional<std::string> GetLoginUrl();
    QRcode*                    GetLoginQRCodeImage();
    bool                       GetLoginQRCode();
    bool                       GetLoginInfo();

private:
    boost::asio::io_context        ioc;        // IO上下文
    boost::asio::ssl::context      ctx;        // SSL上下文
    boost::asio::ip::tcp::resolver resolver;   // DNS解析器

    Url qrCodeUrl;
    Url loginInfoUrl;

    std::string qrCodeKey;
};



#endif   // BILILOGIN_H
