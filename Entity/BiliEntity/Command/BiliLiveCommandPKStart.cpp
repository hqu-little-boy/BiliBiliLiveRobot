//
// Created by zeng on 24-7-10.
//

#include "BiliLiveCommandPKStart.h"

#include "../../../Util/BiliUtil/BiliRequestHeader.h"
#include "../../Global/Config.h"

#include <boost/asio/connect.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/beast/core/buffers_to_string.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/http/dynamic_body.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/read.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/http/write.hpp>

BiliLiveCommandPKStart::BiliLiveCommandPKStart(const nlohmann::json& message)
    : BiliLiveCommandBase(message)
    , oppositeRoomID(0)
    , oppositeAnchor(0, "", 0, 0)
    , totalAudienceNum(0)
    , totalRankcount(0)
    , ioc()
    , ctx(boost::asio::ssl::context::tlsv12_client)
    , resolver(ioc)
{
    LoadMessage(message);
}

std::string BiliLiveCommandPKStart::ToString() const
{
    // return std::format("BiliLiveCommandPKStart: roomID: {}", oppositeRoomID);
    return std::format("BiliLiveCommandPKStart: roomID: {}, anchor: {}, uid: {}, fanCount: {}, "
                       "guardCount: {}, totalAudienceNum: {}, totalRankcount: {}",
                       oppositeRoomID,
                       oppositeAnchor.GetUname(),
                       oppositeAnchor.GetUid(),
                       oppositeAnchor.GetFanCount(),
                       oppositeAnchor.GetGuardCount(),
                       totalAudienceNum,
                       totalRankcount);
}

bool BiliLiveCommandPKStart::LoadMessage(const nlohmann::json& message)
{
    LOG_MESSAGE(LogLevel::Debug, message.dump(4));
    try
    {
        uint64_t initInfoRoomID  = message["data"]["init_info"]["room_id"].get<uint64_t>();
        uint64_t matchInfoRoomID = message["data"]["match_info"]["room_id"].get<uint64_t>();

        if (initInfoRoomID != Config::GetInstance()->GetRoomId())
        {
            this->oppositeRoomID = initInfoRoomID;
        }
        else if (matchInfoRoomID != Config::GetInstance()->GetRoomId())
        {
            this->oppositeRoomID = matchInfoRoomID;
        }
        else
        {
            LOG_MESSAGE(LogLevel::Error, "RoomID not match");
            return false;
        }
    }
    catch (const std::exception& e)
    {
        LOG_MESSAGE(LogLevel::Error, std::format("Failed to load message: {}", e.what()));
        return false;
    }
    if (!GetRoomInit())
    {
        LOG_MESSAGE(LogLevel::Error, "Failed to get user uid");
        return false;
    }
    if (!GetUserInfo())
    {
        LOG_MESSAGE(LogLevel::Error, "Failed to get user uname and uid");
        return false;
    }
    if (!GetTopListInfo())
    {
        LOG_MESSAGE(LogLevel::Error, "Failed to get top list info");
        return false;
    }
    if (!GetRankListInfo())
    {
        LOG_MESSAGE(LogLevel::Error, "Failed to get rank list info");
        return false;
    }
    return true;
}

void BiliLiveCommandPKStart::Run() const
{
    LOG_MESSAGE(LogLevel::Info, this->ToString());
}

bool BiliLiveCommandPKStart::GetRoomInit()
{
    Url url = Url{"api.live.bilibili.com",
                  443,
                  "/room/v1/Room/get_info",
                  {{"room_id", std::to_string(this->oppositeRoomID)}}};
    LOG_VAR(LogLevel::Debug, url.ToString());
    // 解析域名和端口
    const auto results = resolver.resolve(url.GetHost(), std::to_string(url.GetPort()));

    // 创建SSL流并连接
    boost::asio::io_context                                iocHttp;
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket> stream(iocHttp, this->ctx);
    boost::asio::connect(stream.next_layer(), results.begin(), results.end());
    stream.handshake(boost::asio::ssl::stream_base::client);
    // 构建GET请求
    boost::beast::http::request<boost::beast::http::string_body> req{
        boost::beast::http::verb::get, url.GetTargetWithQuery(), 11};
    req.set(boost::beast::http::field::host, url.GetHost());
    req.set(boost::beast::http::field::user_agent,
            BiliRequestHeader::GetInstance()->GetUserAgent());
    // 设置cookie
    req.set(boost::beast::http::field::cookie,
            BiliRequestHeader::GetInstance()->GetBiliCookie().ToString());
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
    try
    {
        if (danmuInfoJson["code"] != 0)
        {
            LOG_VAR(LogLevel::Error, danmuInfoJson["message"].dump(4));
            return false;
        }
        this->oppositeAnchor.SetUid(danmuInfoJson["data"]["uid"].get<uint64_t>());
    }
    catch (const nlohmann::json::exception& e)
    {
        LOG_VAR(LogLevel::Error, e.what());
        return false;
    }
    return true;
}

bool BiliLiveCommandPKStart::GetUserInfo()
{
    Url url = Url{"api.live.bilibili.com",
                  443,
                  "/live_user/v1/Master/info",
                  {{"uid", std::to_string(this->oppositeAnchor.GetUid())}}};
    LOG_VAR(LogLevel::Debug, url.ToString());
    // 解析域名和端口
    const auto results = resolver.resolve(url.GetHost(), std::to_string(url.GetPort()));

    // 创建SSL流并连接
    boost::asio::io_context                                iocHttp;
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket> stream(iocHttp, this->ctx);
    boost::asio::connect(stream.next_layer(), results.begin(), results.end());
    stream.handshake(boost::asio::ssl::stream_base::client);
    // 构建GET请求
    boost::beast::http::request<boost::beast::http::string_body> req{
        boost::beast::http::verb::get, url.GetTargetWithQuery(), 11};
    req.set(boost::beast::http::field::host, url.GetHost());
    req.set(boost::beast::http::field::user_agent,
            BiliRequestHeader::GetInstance()->GetUserAgent());
    // 设置cookie
    req.set(boost::beast::http::field::cookie,
            BiliRequestHeader::GetInstance()->GetBiliCookie().ToString());
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
    try
    {
        if (danmuInfoJson["code"] != 0)
        {
            LOG_VAR(LogLevel::Error, danmuInfoJson["message"].dump(4));
            return false;
        }
        this->oppositeAnchor.SetUid(danmuInfoJson["data"]["info"]["uid"].get<uint64_t>());
        this->oppositeAnchor.SetUname(danmuInfoJson["data"]["info"]["uname"].get<std::string>());
        this->oppositeAnchor.SetFanCount(danmuInfoJson["data"]["follower_num"].get<unsigned>());
    }
    catch (const nlohmann::json::exception& e)
    {
        LOG_VAR(LogLevel::Error, e.what());
        return false;
    }
    return true;
}

bool BiliLiveCommandPKStart::GetTopListInfo()
{
    uint64_t page{1};
    Url      url = Url{
        "api.live.bilibili.com",
        443,
        "/xlive/app-room/v2/guardTab/topList",
             {
            {"page_size", "29"},
            {"roomid", std::to_string(this->oppositeRoomID)},
            {"page", std::to_string(page)},
            {"ruid", std::to_string(this->oppositeAnchor.GetUid())},
        },
    };
    LOG_VAR(LogLevel::Debug, url.ToString());
    // 解析域名和端口
    const auto results = resolver.resolve(url.GetHost(), std::to_string(url.GetPort()));

    // 创建SSL流并连接
    boost::asio::io_context                                iocHttp;
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket> stream(iocHttp, this->ctx);
    boost::asio::connect(stream.next_layer(), results.begin(), results.end());
    stream.handshake(boost::asio::ssl::stream_base::client);
    // 构建GET请求
    boost::beast::http::request<boost::beast::http::string_body> req{
        boost::beast::http::verb::get, url.GetTargetWithQuery(), 11};
    req.set(boost::beast::http::field::host, url.GetHost());
    req.set(boost::beast::http::field::user_agent,
            BiliRequestHeader::GetInstance()->GetUserAgent());
    // 设置cookie
    req.set(boost::beast::http::field::cookie,
            BiliRequestHeader::GetInstance()->GetBiliCookie().ToString());
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
    try
    {
        if (danmuInfoJson["code"] != 0)
        {
            LOG_VAR(LogLevel::Error, danmuInfoJson["message"].dump(4));
            return false;
        }
        this->oppositeAnchor.SetGuardCount(danmuInfoJson["data"]["info"]["num"].get<unsigned>());
    }
    catch (const nlohmann::json::exception& e)
    {
        LOG_VAR(LogLevel::Error, e.what());
        return false;
    }
    return true;
}

bool BiliLiveCommandPKStart::GetRankListInfo()
{

    uint64_t pageSize{1};
    // unsigned totalAudienceNum{0};
    // unsigned totalRankcount{0};
    this->totalAudienceNum = 0;
    this->totalRankcount   = 0;
    // unsigned totalRankcount{0};
    for (uint64_t page{1}; page <= pageSize; page++)
    {
        Url url = Url{
            "api.live.bilibili.com",
            443,
            "/xlive/general-interface/v1/rank/getOnlineGoldRank",
            {
                {"ruid", std::to_string(this->oppositeAnchor.GetUid())},
                {"roomId", std::to_string(this->oppositeRoomID)},
                {"page", std::to_string(page)},
                {"pageSize", "50"},
            },
        };
        LOG_VAR(LogLevel::Debug, url.ToString());
        // 解析域名和端口
        const auto results = resolver.resolve(url.GetHost(), std::to_string(url.GetPort()));

        // 创建SSL流并连接
        boost::asio::io_context                                iocHttp;
        boost::asio::ssl::stream<boost::asio::ip::tcp::socket> stream(iocHttp, this->ctx);
        boost::asio::connect(stream.next_layer(), results.begin(), results.end());
        stream.handshake(boost::asio::ssl::stream_base::client);
        // 构建GET请求
        boost::beast::http::request<boost::beast::http::string_body> req{
            boost::beast::http::verb::get, url.GetTargetWithQuery(), 11};
        req.set(boost::beast::http::field::host, url.GetHost());
        req.set(boost::beast::http::field::user_agent,
                BiliRequestHeader::GetInstance()->GetUserAgent());
        // 设置cookie
        req.set(boost::beast::http::field::cookie,
                BiliRequestHeader::GetInstance()->GetBiliCookie().ToString());
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
        try
        {
            if (danmuInfoJson["code"] != 0)
            {
                LOG_VAR(LogLevel::Error, danmuInfoJson["message"].dump(4));
                return false;
            }
            if (!danmuInfoJson["data"]["OnlineRankItem"].empty() &&
                (danmuInfoJson["data"]["OnlineRankItem"].size() <
                 danmuInfoJson["data"]["onlineNum"].get<unsigned>()))
            {
                pageSize = danmuInfoJson["data"]["onlineNum"].get<unsigned>() /
                           danmuInfoJson["data"]["OnlineRankItem"].size();
                if (danmuInfoJson["data"]["onlineNum"].get<unsigned>() % 50 > 0)
                {
                    pageSize += 1;
                }
            }
            for (const auto& item : danmuInfoJson["data"]["OnlineRankItem"])
            {
                this->totalAudienceNum += 1;
                this->totalRankcount += item["score"].get<unsigned>();
            }
            // this->oppositeAnchor.SetGuardCount(danmuInfoJson["data"]["info"]["num"].get<unsigned>());
        }
        catch (const nlohmann::json::exception& e)
        {
            LOG_VAR(LogLevel::Error, e.what());
            return false;
        }
    }
    return true;
}