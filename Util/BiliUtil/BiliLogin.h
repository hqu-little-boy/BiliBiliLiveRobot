//
// Created by zeng on 24-7-2.
//

#ifndef BILILOGIN_H
#define BILILOGIN_H
#include "../../Base/noncopyable.h"
#include "../../Entity/Global/Url.h"

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/context.hpp>
#include <string>


class BiliLogin : noncopyable
{
public:
    BiliLogin();
    ~BiliLogin() = default;
    bool GetLoginQRCode();
    bool GetLoginInfo();

private:
    Url qrCodeUrl;
    Url loginInfoUrl;

    boost::asio::io_context        ioc;        // IO上下文
    boost::asio::ssl::context      ctx;        // SSL上下文
    boost::asio::ip::tcp::resolver resolver;   // DNS解析器

    std::string qrCodeKey;
};



#endif   // BILILOGIN_H
