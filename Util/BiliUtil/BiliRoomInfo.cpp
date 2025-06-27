//
// Created by zpf on 25-3-12.
//

#include "BiliRoomInfo.h"

#include "../../Entity/Global/Config.h"
#include "BiliRequestHeader.h"

#include <boost/asio/ssl/host_name_verification.hpp>
#include <boost/beast/core/buffers_to_string.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/read.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/http/verb.hpp>
#include <boost/beast/http/write.hpp>
BiliRoomInfo::BiliRoomInfo(uint64_t roomId)
    : url("api.live.bilibili.com", 443, "/room/v1/Room/get_info", {{"id", std::to_string(roomId)}})
    , roomId(roomId)
    , attentionCount(0)
    , onlineCount(0)
    , isLiving(false)
    , ctx(boost::asio::ssl::context::tlsv12_client)
    , resolver(ioc)
    , lastAttentionTime(
          std::chrono::time_point<std::chrono::system_clock>::min())   // 初始化为最小时间点
{
}
bool BiliRoomInfo::Init()
{
    ctx.set_verify_mode(boost::asio::ssl::verify_peer);
    return true;
}
bool BiliRoomInfo::GetRoomInfo()
{
    // #ifndef TEST
    //     // 读取json文件
    //     std::ifstream ifs("/home/zpf/git/BiliBiliLiveRobot/test.json");
    //     if (!ifs.is_open())
    //     {
    //         LOG_MESSAGE(LogLevel::Error, "Failed to open file");
    //         return false;
    //     }
    //     auto resStr =
    //         std::string((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    // #else
    boost::asio::ssl::stream<boost::beast::tcp_stream> stream(ioc, ctx);
    // 设置SNI主机名
    if (!SSL_set_tlsext_host_name(stream.native_handle(), url.GetHost().c_str()))
    {
        LOG_VAR(LogLevel::Error, "Failed to set SNI Hostname");
        return false;
    }
    // 设置对等验证回调，并指定预期的主机名
    boost::system::error_code ecVerifyCallback;
    stream.set_verify_callback(boost::asio::ssl::host_name_verification(url.GetHost().c_str()),
                               ecVerifyCallback);
    if (ecVerifyCallback)
    {
        LOG_VAR(LogLevel::Error,
                fmt::format("Failed to set verify callback: {}", ecVerifyCallback.message()));
        return false;
    }
    // 解析域名和端口
    LOG_VAR(
        LogLevel::Debug,
        fmt::format("Resolving {}:{}", this->url.GetHost(), std::to_string(this->url.GetPort())));
    boost::system::error_code ecResolver;
    const auto                results{this->resolver.resolve(
        this->url.GetHost(), std::to_string(this->url.GetPort()), ecResolver)};
    if (ecResolver)
    {
        LOG_VAR(LogLevel::Error,
                fmt::format("Failed to resolve {}: {}", this->url.GetHost(), ecResolver.message()));
        return false;
    }

    // 连接到IP地址

    boost::system::error_code ecConnect;
    boost::beast::get_lowest_layer(stream).connect(results, ecConnect);   // 连接到IP地址
    if (ecConnect)
    {
        LOG_VAR(LogLevel::Error, fmt::format("Failed to connect: {}", ecConnect.message()));
        return false;
    }

    // 执行SSL握手

    boost::system::error_code ecHandshake;
    stream.handshake(boost::asio::ssl::stream_base::client, ecHandshake);   // 进行SSL握手
    if (ecHandshake)
    {
        LOG_VAR(LogLevel::Error,
                fmt::format("Failed to perform SSL handshake: {}", ecHandshake.message()));
        return false;
    }
    // 构建请求

    boost::beast::http::request<boost::beast::http::string_body> req{
        boost::beast::http::verb::post, this->url.GetTargetWithQuery(), 11};
    // 设置请求头
    req.set(boost::beast::http::field::host, this->url.GetHost());
    req.set(boost::beast::http::field::user_agent,
            BiliRequestHeader::GetInstance()->GetUserAgent());
    req.set(boost::beast::http::field::content_type, "application/json");
    req.set(boost::beast::http::field::cookie,
            BiliRequestHeader::GetInstance()->GetBiliCookie().ToString());

    // 发送HTTP请求
    boost::system::error_code ecSend;
    boost::beast::http::write(stream, req, ecSend);
    if (ecSend)
    {
        LOG_VAR(LogLevel::Error, fmt::format("Failed to send request: {}", ecSend.message()));
        return false;
    }
    // 读取响应
    boost::beast::flat_buffer                                     buffer;
    boost::beast::http::response<boost::beast::http::string_body> res;

    boost::system::error_code ecResponse;
    boost::beast::http::read(stream, buffer, res, ecResponse);
    if (ecResponse)
    {
        LOG_VAR(LogLevel::Error, fmt::format("Failed to read response: {}", ecResponse.message()));
        return false;
    }
    if (res.result() != boost::beast::http::status::ok)
    {
        return false;
    }
    // 解析json
    auto resStr = res.body();
    // #endif
    nlohmann::json roomInfoJson = nlohmann::json::parse(resStr);
    LOG_VAR(LogLevel::Debug, roomInfoJson.dump(-1));
    if (roomInfoJson["code"] != 0 || roomInfoJson["msg"] != "ok")
    {
        return false;
    }
    this->attentionCount = roomInfoJson["data"]["attention"].get<uint64_t>();
    this->onlineCount    = roomInfoJson["data"]["online"].get<uint64_t>();
    this->isLiving       = roomInfoJson["data"]["live_status"].get<uint64_t>() == 1;
    LOG_MESSAGE(LogLevel::Debug,
                fmt::format("RoomId: {}, Attention: {}, Online: {}, IsLiving: {}",
                            this->roomId,
                            this->attentionCount,
                            this->onlineCount,
                            this->isLiving));
    this->lastAttentionTime = std::chrono::system_clock::now();
    return true;
}
uint64_t BiliRoomInfo::GetRoomId() const
{
    return this->roomId;
}
void BiliRoomInfo::SetRoomId(uint64_t roomId)
{
    this->roomId = roomId;
    this->url.SetQuery({{"id", std::to_string(roomId)}});
}
uint64_t BiliRoomInfo::GetAttentionCount()
{
    // 如果超过2分钟没有获取到最新的房间信息，则重新获取
    if (!this->GetRoomInfo())
    {
        LOG_MESSAGE(LogLevel::Error, "Failed to get room info");
        return false;
    }
    return this->attentionCount;
}
uint64_t BiliRoomInfo::GetOnlineCount()
{
    // 如果超过2分钟没有获取到最新的房间信息，则重新获取
    if (!this->GetRoomInfo())
    {
        LOG_MESSAGE(LogLevel::Error, "Failed to get room info");
        return false;
    }
    return this->onlineCount;
}
bool BiliRoomInfo::IsLiving()
{
    if (this->lastAttentionTime + std::chrono::minutes(2) < std::chrono::system_clock::now())
    {
        // 如果超过2分钟没有获取到最新的房间信息，则重新获取
        if (!this->GetRoomInfo())
        {
            LOG_MESSAGE(LogLevel::Error, "Failed to get room info");
            return false;
        }
    }
    return this->isLiving;
}