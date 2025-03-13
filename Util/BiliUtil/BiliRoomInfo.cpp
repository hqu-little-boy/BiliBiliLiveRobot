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
#include <boost/beast/http/message_fwd.hpp>
#include <boost/beast/http/read.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/http/string_body_fwd.hpp>
#include <boost/beast/http/verb.hpp>
#include <boost/beast/http/write.hpp>
BiliRoomInfo::BiliRoomInfo(uint64_t roomId)
    : roomId(roomId)
    , ctx(boost::asio::ssl::context::tlsv12_client)
    , resolver(ioc)
    , url("api.live.bilibili.com",
          443,
          "/room/v1/Room/get_info",
          {{{"id", std::to_string(Config::GetInstance()->GetRoomId())}}})
// , stream(ioc, ctx)
{
}
bool BiliRoomInfo::Init()
{
    ctx.set_verify_mode(boost::asio::ssl::verify_peer);
    return true;
}
bool BiliRoomInfo::GetRoomInfo()
{
    // #ifdef TEST
    //     // 读取json文件
    //     std::ifstream ifs("/home/zpf/git/BiliBiliLiveRobot/test.json");
    //     if (!ifs.is_open())
    //     {
    //         LOG_MESSAGE(LogLevel::Error, "Failed to open file");
    //         return false;
    //     }
    //     auto resStr = std::string((std::istreambuf_iterator<char>(ifs)),
    //     std::istreambuf_iterator<char>());
    // #else
    boost::asio::ssl::stream<boost::beast::tcp_stream> stream(ioc, ctx);
    if (!SSL_set_tlsext_host_name(stream.native_handle(), url.GetHost().c_str()))
    {
        throw boost::beast::system_error(static_cast<int>(::ERR_get_error()),
                                         boost::asio::error::get_ssl_category());
    }
    // 设置对等验证回调，并指定预期的主机名
    stream.set_verify_callback(boost::asio::ssl::host_name_verification(url.GetHost().c_str()));
    // 解析域名和端口
    const auto results{
        this->resolver.resolve(this->url.GetHost(), std::to_string(this->url.GetPort()))};
    boost::beast::get_lowest_layer(stream).connect(results);   // 连接到IP地址
    stream.handshake(boost::asio::ssl::stream_base::client);   // 进行SSL握手
    // 构建请求
    boost::beast::http::request<boost::beast::http::string_body> req{
        boost::beast::http::verb::post, this->url.GetTargetWithQuery(), 11};
    req.set(boost::beast::http::field::host, this->url.GetHost());
    req.set(boost::beast::http::field::user_agent,
            BiliRequestHeader::GetInstance()->GetUserAgent());
    req.set(boost::beast::http::field::content_type, "application/json");
    req.set(boost::beast::http::field::cookie,
            BiliRequestHeader::GetInstance()->GetBiliCookie().ToString());

    // 发送HTTP请求
    boost::beast::http::write(stream, req);
    // 读取响应
    boost::beast::flat_buffer                                     buffer;
    boost::beast::http::response<boost::beast::http::string_body> res;
    boost::beast::http::read(stream, buffer, res);
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
    LOG_MESSAGE(LogLevel::Info,
                fmt::format("RoomId: {}, Attention: {}, Online: {}, IsLiving: {}",
                            this->roomId,
                            this->attentionCount,
                            this->onlineCount,
                            this->isLiving));
    return true;
}
uint64_t BiliRoomInfo::GetRoomId() const
{
    return this->roomId;
}
uint64_t BiliRoomInfo::GetAttentionCount() const
{
    return this->attentionCount;
}
uint64_t BiliRoomInfo::GetOnlineCount() const
{
    return this->onlineCount;
}
bool BiliRoomInfo::IsLiving() const
{
    return this->isLiving;
}