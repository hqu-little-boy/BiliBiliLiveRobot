//
// Created by zeng on 24-7-10.
//

#include "BiliLiveCommandPKStart.h"

#include "../../../Util/BiliUtil/BiliRequestHeader.h"
#include "../../Global/Config.h"
#include "../../MessageDeque/MessageDeque.h"

#include <boost/asio/connect.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/beast/core/buffers_to_string.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/http/dynamic_body.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/read.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/http/write.hpp>

uint64_t BiliLiveCommandPKStart::lastPKRoomID{0};
BiliLiveCommandPKStart::BiliLiveCommandPKStart()
    : BiliLiveCommandBase()
    , oppositeRoomID(0)
    , oppositeAnchor(0, "", 0, 0)
    , totalAudienceNum(0)
    , totalRankcount(0)
    , ioc()
    , ctx(boost::asio::ssl::context::tlsv12_client)
    , resolver(ioc)
{
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
    std::unique_lock<std::mutex> lck(this->mtx);
    LOG_MESSAGE(LogLevel::Debug, message.dump(-1));
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
    if (this->oppositeRoomID == lastPKRoomID)
    {
        LOG_MESSAGE(LogLevel::Error, "Repeated PK roomID");
        return false;
    }
    lastPKRoomID = this->oppositeRoomID;
    lck.unlock();
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
    if (Config::GetInstance()->CanPKNotice())
    {
        return;
    }
    MessageDeque::GetInstance()->PushWaitedMessage(std::format("PK开始，{}，粉丝数：{}",
                                                               this->oppositeAnchor.GetUname(),
                                                               this->oppositeAnchor.GetFanCount()));
    MessageDeque::GetInstance()->PushWaitedMessage(
        std::format("总舰长数：{}，总观众数：{}，总亲密度：{}",
                    this->oppositeAnchor.GetGuardCount(),
                    this->totalAudienceNum,
                    this->totalRankcount));
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
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket> stream(this->ioc, this->ctx);
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
    std::string    resStr       = boost::beast::buffers_to_string(res.body().data());
    nlohmann::json roomInfoJson = nlohmann::json::parse(resStr);
    LOG_VAR(LogLevel::Debug, roomInfoJson.dump(-1));
    try
    {
        if (roomInfoJson["code"] != 0)
        {
            LOG_VAR(LogLevel::Error, roomInfoJson["message"].dump(4));
            return false;
        }
        this->oppositeAnchor.SetUid(roomInfoJson["data"]["uid"].get<uint64_t>());
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
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket> stream(this->ioc, this->ctx);
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
    std::string    resStr       = boost::beast::buffers_to_string(res.body().data());
    nlohmann::json userInfoJson = nlohmann::json::parse(resStr);
    LOG_VAR(LogLevel::Debug, userInfoJson.dump(-1));
    try
    {
        if (userInfoJson["code"] != 0)
        {
            LOG_VAR(LogLevel::Error, userInfoJson["message"].dump(4));
            return false;
        }
        this->oppositeAnchor.SetUid(userInfoJson["data"]["info"]["uid"].get<uint64_t>());
        this->oppositeAnchor.SetUname(userInfoJson["data"]["info"]["uname"].get<std::string>());
        this->oppositeAnchor.SetFanCount(userInfoJson["data"]["follower_num"].get<unsigned>());
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
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket> stream(this->ioc, this->ctx);
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
    std::string    resStr          = boost::beast::buffers_to_string(res.body().data());
    nlohmann::json topListInfoJson = nlohmann::json::parse(resStr);
    LOG_VAR(LogLevel::Debug, topListInfoJson.dump(-1));
    try
    {
        if (topListInfoJson["code"] != 0)
        {
            LOG_VAR(LogLevel::Error, topListInfoJson["message"].dump(4));
            return false;
        }
        this->oppositeAnchor.SetGuardCount(topListInfoJson["data"]["info"]["num"].get<unsigned>());
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
        boost::asio::ssl::stream<boost::asio::ip::tcp::socket> stream(this->ioc, this->ctx);
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
        std::string    resStr       = boost::beast::buffers_to_string(res.body().data());
        nlohmann::json rankInfoJson = nlohmann::json::parse(resStr);
        LOG_VAR(LogLevel::Debug, rankInfoJson.dump(-1));
        try
        {
            if (rankInfoJson["code"] != 0)
            {
                LOG_VAR(LogLevel::Error, rankInfoJson["message"].dump(4));
                return false;
            }
            if (!rankInfoJson["data"]["OnlineRankItem"].empty() &&
                (rankInfoJson["data"]["OnlineRankItem"].size() <
                 rankInfoJson["data"]["onlineNum"].get<unsigned>()))
            {
                pageSize = rankInfoJson["data"]["onlineNum"].get<unsigned>() /
                           rankInfoJson["data"]["OnlineRankItem"].size();
                if (rankInfoJson["data"]["onlineNum"].get<unsigned>() % 50 > 0)
                {
                    pageSize += 1;
                }
            }
            for (const auto& item : rankInfoJson["data"]["OnlineRankItem"])
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