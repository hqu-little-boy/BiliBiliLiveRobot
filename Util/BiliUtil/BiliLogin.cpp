//
// Created by zeng on 24-7-2.
//

#include "BiliLogin.h"

#include "../../Entity/Global/Logger.h"
#include "BiliRequestHeader.h"
#include "qrencode.h"

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/beast/core/buffers_to_string.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/http/dynamic_body.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/read.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/http/write.hpp>
#include <iostream>
#include <nlohmann/json.hpp>
#include <thread>

BiliLogin::BiliLogin()
    : ioc()
    , ctx(boost::asio::ssl::context::sslv23)
    , resolver(ioc)
    , qrCodeUrl{"passport.bilibili.com", 443, "/x/passport-login/web/qrcode/generate"}
    , loginInfoUrl{"passport.bilibili.com", 443, "/x/passport-login/web/qrcode/poll"}
{
}

bool BiliLogin::GetLoginQRCode()
{
    // 解析域名和端口
    const auto results = this->resolver.resolve(this->qrCodeUrl.GetHost(),
                                                std::to_string(this->qrCodeUrl.GetPort()));

    boost::asio::ssl::stream<boost::asio::ip::tcp::socket> stream(this->ioc, this->ctx);
    boost::asio::connect(stream.next_layer(), results.begin(), results.end());
    stream.handshake(boost::asio::ssl::stream_base::client);
    // 构建GET请求
    boost::beast::http::request<boost::beast::http::string_body> req{
        boost::beast::http::verb::get, qrCodeUrl.GetTarget(), 11};
    req.set(boost::beast::http::field::host, qrCodeUrl.GetHost());
    req.set(boost::beast::http::field::user_agent,
            BiliRequestHeader::GetInstance()->GetUserAgent());

    // 发送请求
    boost::beast::http::write(stream, req);

    // 接收响应
    boost::beast::flat_buffer                                      buffer;
    boost::beast::http::response<boost::beast::http::dynamic_body> res;
    boost::beast::http::read(stream, buffer, res);
    if (res.result() != boost::beast::http::status::ok)
    {
        LOG_VAR(LogLevel::Error, res.result_int());
        return false;
    }
    // 解析json
    std::string    resStr        = boost::beast::buffers_to_string(res.body().data());
    nlohmann::json danmuInfoJson = nlohmann::json::parse(resStr);
    LOG_VAR(LogLevel::Debug, danmuInfoJson.dump(4));
    if (danmuInfoJson["code"] != 0)
    {
        LOG_VAR(LogLevel::Error, danmuInfoJson["message"].dump(4));
        return false;
    }
    std::string url;
    try
    {
        this->qrCodeKey = danmuInfoJson["data"]["qrcode_key"].get<std::string>();
        url             = danmuInfoJson["data"]["url"].get<std::string>();
    }
    catch (const nlohmann::json::exception& e)
    {
        LOG_VAR(LogLevel::Error, e.what());
        return false;
    }
    this->loginInfoUrl.SetQuery({{"qrcode_key", this->qrCodeKey}});
    QRcode* qrcode = QRcode_encodeString(url.data(), 2, QR_ECLEVEL_L, QR_MODE_8, 1);
    if (qrcode == nullptr)
    {
        LOG_MESSAGE(LogLevel::Error, "Failed to encode QRCode");
        return false;
    }
    // 控制台打印二维码
    for (int y = 0; y < qrcode->width; y++)
    {
        for (int x = 0; x < qrcode->width; x++)
        {
            std::cout << (qrcode->data[y * qrcode->width + x] & 0x01 ? "\033[47m  \033[0m"
                                                                     : "\033[40m  \033[0m");
        }
        std::cout << std::endl;
    }
    delete qrcode;

    for (int i = 0; i < 15; i++)
    {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        if (!this->GetLoginInfo())
        {
            continue;
        }
        else
        {
            return true;
        }
    }
    return false;
}

bool BiliLogin::GetLoginInfo()
{
    // 解析域名和端口
    const auto results = this->resolver.resolve(this->loginInfoUrl.GetHost(),
                                                std::to_string(this->loginInfoUrl.GetPort()));
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket> stream(this->ioc, this->ctx);
    boost::asio::connect(stream.next_layer(), results.begin(), results.end());
    stream.handshake(boost::asio::ssl::stream_base::client);
    // 构建Get请求
    boost::beast::http::request<boost::beast::http::string_body> req{
        boost::beast::http::verb::get, loginInfoUrl.GetTargetWithQuery(), 11};
    req.set(boost::beast::http::field::host, loginInfoUrl.GetHost());
    req.set(boost::beast::http::field::user_agent,
            BiliRequestHeader::GetInstance()->GetUserAgent());


    // 发送请求
    boost::beast::http::write(stream, req);

    // 接收响应
    boost::beast::flat_buffer                                      buffer;
    boost::beast::http::response<boost::beast::http::dynamic_body> res;
    boost::beast::http::read(stream, buffer, res);
    if (res.result() != boost::beast::http::status::ok)
    {
        LOG_VAR(LogLevel::Error, res.result_int());
        return false;
    }
    // 解析json
    std::string    resStr        = boost::beast::buffers_to_string(res.body().data());
    nlohmann::json danmuInfoJson = nlohmann::json::parse(resStr);
    // LOG_VAR(LogLevel::Debug, danmuInfoJson.dump(4));
    if (danmuInfoJson["code"].get<int>() != 0)
    {
        LOG_VAR(LogLevel::Error, danmuInfoJson["message"].dump(4));
        return false;
    }
    // 判断是否扫描登录
    int registerCode = danmuInfoJson["data"]["code"].get<int>();
    // if (registerCode == 86038)   // 二维码已失效
    // {
    //     LOG_MESSAGE(LogLevel::Error, "二维码已失效");
    //     return false;
    // }
    // else if (registerCode == 86090)   // 已扫描但未确认
    // {
    //     LOG_MESSAGE(LogLevel::Error, "等待确认");
    //     return false;
    // }
    // else if (registerCode == 86101)   // 未扫描
    // {
    //     LOG_MESSAGE(LogLevel::Error, "等待扫描");
    //     return false;
    // }
    if (registerCode != 0)
    {
        // LOG_MESSAGE(LogLevel::Error, "未登录");
        return false;
    }
    // 打印cookie
    std::vector<std::string> cookies;

    for (const auto& header : res.base())
    {
        std::vector<std::string> tokens;
        auto                     name = header.name();
        if (name == boost::beast::http::field::set_cookie)
        {
            boost::split(tokens, header.value(), boost::is_any_of("; "));
            cookies.insert(cookies.end(), tokens.begin(), tokens.end());
        }
    }
    nlohmann::json cookieJson;
    for (const auto& cookie : cookies)
    {
        if (cookies.empty() || !cookie.contains("="))
        {
            continue;
        }
        // std::cout << cookie << std::endl;
        std::vector<std::string> tokens;
        boost::split(tokens, cookie, boost::is_any_of("="));
        if (tokens.size() != 2)
        {
            continue;
        }
        cookieJson[tokens[0]] = tokens[1];
    }
    LOG_VAR(LogLevel::Debug, cookieJson.dump(4));
    if (cookieJson.empty())
    {
        LOG_MESSAGE(LogLevel::Error, "Failed to get cookie");
        return false;
    }
    // 保存cookie，判断cookie文件夹是否存在
    if (!std::filesystem::exists("cookie"))
    {
        // 创建文件夹
        if (!std::filesystem::create_directories("cookie"))
        {
            LOG_MESSAGE(LogLevel::Error, "Failed to create directory");
            return false;
        }
    }
    std::ofstream ofs("cookie/bili_cookie.json");
    if (!ofs.is_open())
    {
        LOG_MESSAGE(LogLevel::Error, "Failed to open file");
        return false;
    }
    // 连接https://api.bilibili.com/x/frontend/finger/spi获取buvid
    // 解析域名和端口
    const auto buvidResults = this->resolver.resolve("api.bilibili.com", "443");
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket> buvidStream(this->ioc, this->ctx);
    boost::asio::connect(buvidStream.next_layer(), buvidResults.begin(), buvidResults.end());
    buvidStream.handshake(boost::asio::ssl::stream_base::client);
    // 构建Get请求
    boost::beast::http::request<boost::beast::http::string_body> buvidReq{
        boost::beast::http::verb::get, "/x/frontend/finger/spi", 11};
    buvidReq.set(boost::beast::http::field::host, "api.bilibili.com");
    buvidReq.set(boost::beast::http::field::user_agent,
                 BiliRequestHeader::GetInstance()->GetUserAgent());
    buvidReq.set(boost::beast::http::field::cookie,
                 "SESSDATA=" + cookieJson["SESSDATA"].get<std::string>());
    // 设置所有res.base()中的cookie
    // for (const auto& header : res.base())
    // {
    //     if (header.name() == boost::beast::http::field::set_cookie)
    //     {
    //         buvidReq.set(boost::beast::http::field::set_cookie, header.value());
    //     }
    // }
    // 发送请求
    boost::beast::http::write(buvidStream, buvidReq);
    // 接收响应
    boost::beast::flat_buffer                                      buvidBuffer;
    boost::beast::http::response<boost::beast::http::dynamic_body> buvidRes;
    boost::beast::http::read(buvidStream, buvidBuffer, buvidRes);
    if (buvidRes.result() != boost::beast::http::status::ok)
    {
        LOG_VAR(LogLevel::Error, buvidRes.result_int());
        return false;
    }
    // 解析json
    std::string    buvidResStr = boost::beast::buffers_to_string(buvidRes.body().data());
    nlohmann::json buvidJson   = nlohmann::json::parse(buvidResStr);
    LOG_VAR(LogLevel::Debug, buvidJson.dump(4));

    try
    {
        if (buvidJson["code"].get<int>() != 0)
        {
            LOG_VAR(LogLevel::Error, buvidJson["message"].dump(4));
            return false;
        }
        cookieJson["buvid3"] = buvidJson["data"]["b_3"].get<std::string>();
        cookieJson["buvid4"] = buvidJson["data"]["b_4"].get<std::string>();
    }
    catch (const nlohmann::json::exception& e)
    {
        LOG_VAR(LogLevel::Error, e.what());
        return false;
    }

    ofs << cookieJson.dump(4);
    ofs.close();
    return BiliRequestHeader::GetInstance()->LoadBiliCookieByJson(cookieJson);
}