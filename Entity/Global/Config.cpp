//
// Created by zeng on 24-6-26.
//

#include "Config.h"

#include "../../Util/BiliUtil/BiliRequestHeader.h"
#include "Logger.h"
#include "TimeStamp.h"
#include "nlohmann/json.hpp"

#include <boost/asio/ssl/host_name_verification.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/http.hpp>
#include <boost/regex.hpp>
#include <fstream>
#include <random>

constexpr std::array<uint8_t, 64>
    mixinKeyEncTab = {46, 47, 18, 2,  53, 8,  23, 32, 15, 50, 10, 31, 58, 3,  45, 35, 27,
                      43, 5,  49, 33, 9,  42, 19, 29, 28, 14, 39, 12, 38, 41, 13, 37, 48,
                      7,  16, 24, 55, 40, 61, 26, 17, 0,  1,  60, 51, 30, 4,  22, 25, 54,
                      21, 56, 59, 6,  63, 57, 62, 11, 36, 20, 34, 44, 52};   // wbi混淆密钥表

Config* Config::pInstance{new Config()};
Config* Config::GetInstance()
{
    if (pInstance == nullptr)
    {
        assert(false);
        pInstance = new Config();
    }
    return pInstance;
}

void Config::DestroyInstance()
{
    if (pInstance != nullptr)
    {
        delete pInstance;
        pInstance = nullptr;
    }
}

bool Config::LoadFromJson(std::string_view jsonPath)
{
    if (jsonPath.empty())
    {
        LOG_MESSAGE(LogLevel::Error, "jsonPath is empty");
        throw std::runtime_error("jsonPath is empty");
        return false;
    }

    if (!std::filesystem::exists(jsonPath))
    {
        LOG_MESSAGE(LogLevel::Error, fmt::format("File not exists: {}", jsonPath));
        throw std::runtime_error(fmt::format("File not exists: {}", jsonPath));
        return false;
    }

    std::ifstream ifs(jsonPath.data());
    if (!ifs.is_open())
    {
        LOG_MESSAGE(LogLevel::Error, fmt::format("Failed to open file: {}", jsonPath));
        throw std::runtime_error(fmt::format("Failed to open file: {}", jsonPath));
        return false;
    }

    try
    {
        // 从文件中读取json
        nlohmann::json configJson;
        ifs >> configJson;
        this->roomId            = configJson["roomId"].get<uint64_t>();
        int logLevelInt         = configJson["logLevel"].get<unsigned>();
        this->logLevel          = static_cast<LogLevel>(logLevelInt);
        this->danmuSeverConfUrl = Url("api.live.bilibili.com",
                                      443,
                                      "/xlive/web-room/v1/index/getDanmuInfo",
                                      {{"id", std::to_string(this->roomId)}, {"type", "0"}});
        this->logPath           = fmt::format(
            "{}.{}", configJson["logPath"].get<std::string>(), TimeStamp::Now().ToString());
        this->danmuLength        = configJson["danmuLength"].get<uint8_t>();
        this->canPKNotice        = configJson["canPKNotice"].get<bool>();
        this->canGuardNotice     = configJson["canGuardNotice"].get<bool>();
        this->canThanksGift      = configJson["canThanksGift"].get<bool>();
        this->canSuperChatNotice = configJson["canSuperChatNotice"].get<bool>();
        this->thanksGiftTimeout  = configJson["thanksGiftTimeout"].get<uint8_t>();
        this->canDrawByLot       = configJson["canDrawByLot"].get<bool>();
        this->drawByLotList      = configJson["drawByLotList"].get<std::vector<std::string>>();
        this->canEntryNotice     = configJson["canEntryNotice"].get<bool>();
        this->normalEntryNoticeList =
            configJson["normalEntryNoticeList"].get<std::vector<std::string>>();
        this->guardEntryNoticeList =
            configJson["guardEntryNoticeList"].get<std::vector<std::string>>();

        this->canThanksFocus = configJson["canThanksFocus"].get<bool>();
        this->canThanksShare = configJson["canThanksShare"].get<bool>();
    }
    catch (const nlohmann::json::exception& e)
    {
        LOG_MESSAGE(LogLevel::Error, fmt::format("Failed to parse json: {}", e.what()));
        // throw std::runtime_error("Failed to parse json: " + std::string(e.what()));
        return false;
    }
    return true;
}
bool Config::LoadUID()
{
    boost::asio::ssl::stream<boost::beast::tcp_stream> stream(ioc, ctx);
    if (!SSL_set_tlsext_host_name(stream.native_handle(), this->uidUrl.GetHost().c_str()))
    {
        throw boost::beast::system_error(static_cast<int>(::ERR_get_error()),
                                         boost::asio::error::get_ssl_category());
    }
    // 设置对等验证回调，并指定预期的主机名
    stream.set_verify_callback(
        boost::asio::ssl::host_name_verification(this->uidUrl.GetHost().c_str()));
    // 解析域名和端口
    const auto results{
        this->resolver.resolve(this->uidUrl.GetHost(), std::to_string(this->uidUrl.GetPort()))};
    boost::beast::get_lowest_layer(stream).connect(results);   // 连接到IP地址
    stream.handshake(boost::asio::ssl::stream_base::client);   // 进行SSL握手
    // 构建请求
    boost::beast::http::request<boost::beast::http::string_body> req{
        boost::beast::http::verb::get, this->uidUrl.GetTarget(), 11};
    req.set(boost::beast::http::field::host, this->uidUrl.GetHost());
    req.set(boost::beast::http::field::user_agent,
            BiliRequestHeader::GetInstance()->GetUserAgent());
    // req.set(boost::beast::http::field::content_type, "application/json");
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
        LOG_MESSAGE(LogLevel::Error,
                    fmt::format("Failed to get UID, status: {}", res.result_int()));
        return false;
    }
    // 解析json
    auto resStr = res.body();
    // #endif
    nlohmann::json roomInfoJson = nlohmann::json::parse(resStr);
    LOG_VAR(LogLevel::Debug, roomInfoJson.dump(-1));
    if (!roomInfoJson.contains("code") || !roomInfoJson["code"].is_number_integer() ||
        roomInfoJson["code"].get<int>() != 0)
    {
        LOG_MESSAGE(LogLevel::Error, "Failed to get UID");
        LOG_VAR(LogLevel::Error, roomInfoJson.dump(-1));
        return false;
    }

    // 检查 data.isLogin 字段是否存在且为布尔值
    if (!roomInfoJson.contains("data") || !roomInfoJson["data"].contains("isLogin") ||
        !roomInfoJson["data"]["isLogin"].is_boolean() ||
        !roomInfoJson["data"]["isLogin"].get<bool>())
    {
        LOG_MESSAGE(LogLevel::Error, "User is not logged in");
        LOG_VAR(LogLevel::Error, roomInfoJson.dump(-1));
    }
    // 检查 data.mid 字段是否存在且为整数
    if (!roomInfoJson["data"].contains("mid") || !roomInfoJson["data"]["mid"].is_number_integer())
    {
        LOG_MESSAGE(LogLevel::Error, "Failed to get UID");
        LOG_VAR(LogLevel::Error, roomInfoJson.dump(-1));
        return false;
    }
    this->robotUID = roomInfoJson["data"]["mid"].get<uint64_t>();
    LOG_VAR(LogLevel::Info, this->robotUID);
    // 检查 wbi_img 及其子字段是否存在
    if (!roomInfoJson["data"].contains("wbi_img") ||
        !roomInfoJson["data"]["wbi_img"].contains("img_url") ||
        !roomInfoJson["data"]["wbi_img"]["img_url"].is_string() ||
        !roomInfoJson["data"]["wbi_img"].contains("sub_url") ||
        !roomInfoJson["data"]["wbi_img"]["sub_url"].is_string())
    {
        LOG_MESSAGE(LogLevel::Error, "Failed to get WBI Mix Key");
        LOG_VAR(LogLevel::Error, roomInfoJson.dump(-1));
        return false;
    }
    // 获取 wbi_img 的 img_url 和 sub_url
    auto imgUrl{roomInfoJson["data"]["wbi_img"]["img_url"].get<std::string>()};
    auto subUrl{roomInfoJson["data"]["wbi_img"]["sub_url"].get<std::string>()};
    // 正则表达式，提取 URL 中的 32 位十六进制字符串
    const boost::regex re(R"(/(\w{32})\.png)");   // 使用原始字符串避免转义
    boost::smatch      matches;
    // 提取 img_url 中的 32 位字符串
    if (!boost::regex_search(imgUrl, matches, re) || matches.size() < 2)
    {
        LOG_MESSAGE(LogLevel::Error, "img_url 中未找到32位字符串");
        return false;
    }
    std::string wbiImgBa = matches[1];

    // 提取 sub_url 中的 32 位字符串
    if (!boost::regex_search(subUrl, matches, re) || matches.size() < 2)
    {
        LOG_MESSAGE(LogLevel::Error, "sub_url 中未找到32位字符串");
        return false;
    }
    std::string wbiSubBa = matches[1];

    // 拼接两个字符串为 64 位
    std::string wbiImgSub = wbiImgBa + wbiSubBa;
    if (wbiImgSub.size() != 64)
    {
        LOG_MESSAGE(LogLevel::Error, "拼接后字符串长度不为64");
        return false;
    }

    // 使用索引表重排字符
    std::string newString;
    for (uint8_t idx : mixinKeyEncTab)
    {
        if (idx < 0 || idx >= static_cast<int>(wbiImgSub.size()))
        {
            LOG_MESSAGE(LogLevel::Error, "索引越界: " + std::to_string(idx));
            return false;
        }
        newString += wbiImgSub[idx];
    }
    // newString的前32位赋值给this->wbiMixKey
    this->wbiMixKey = newString.substr(0, 32);
    LOG_VAR(LogLevel::Info, fmt::format("WBI Mix Key: {}", this->wbiMixKey));
    return true;
}

uint64_t Config::GetRoomId() const
{
    return this->roomId;
}

const Url& Config::GetDanmuSeverConfUrl() const
{
    return this->danmuSeverConfUrl;
}

LogLevel Config::GetLogLevel() const
{
    return this->logLevel;
}

const std::string& Config::GetLogPath() const
{
    return this->logPath;
}

uint8_t Config::GetDanmuLength() const
{
    return this->danmuLength;
}

bool Config::CanPKNotice() const
{
    return this->canPKNotice;
}

bool Config::CanGuardNotice() const
{
    return this->canGuardNotice;
}

bool Config::CanThanksGift() const
{
    return this->canThanksGift;
}

bool Config::CanSuperChatNotice() const
{
    return this->canSuperChatNotice;
}

uint8_t Config::GetThanksGiftTimeout() const
{
    return this->thanksGiftTimeout;
}

bool Config::CanDrawByLot() const
{
    return this->canDrawByLot;
}

const std::string& Config::GetDrawByLotWord() const
{
    std::random_device seed_gen;
    std::seed_seq      seed_sequence{seed_gen(), seed_gen(), seed_gen(), seed_gen()};
    std::mt19937       engine(seed_sequence);
    return this->drawByLotList[engine() % this->drawByLotList.size()];
}

bool Config::CanThanksFocus() const
{
    return this->canThanksFocus;
}

void Config::SetCanThanksFocus(bool canThanksFocus)
{
    this->canThanksFocus = canThanksFocus;
}

bool Config::CanThanksShare() const
{
    return this->canThanksShare;
}

void Config::SetCanThanksShare(bool canThanksShare)
{
    this->canThanksShare = canThanksShare;
}

bool Config::CanEntryNotice() const
{
    return this->canEntryNotice;
}

void Config::SetCanEntryNotice(bool canEntryNotice)
{
    this->canEntryNotice = canEntryNotice;
}

const std::string& Config::GetNormalEntryNoticeWord() const
{
    std::random_device seed_gen;
    std::seed_seq      seed_sequence{seed_gen(), seed_gen(), seed_gen(), seed_gen()};
    std::mt19937       engine(seed_sequence);
    return this->normalEntryNoticeList[engine() % this->normalEntryNoticeList.size()];
}

const std::string& Config::GetGuardEntryNoticeWord() const
{
    std::random_device seed_gen;
    std::seed_seq      seed_sequence{seed_gen(), seed_gen(), seed_gen(), seed_gen()};
    std::mt19937       engine(seed_sequence);
    return this->guardEntryNoticeList[engine() % this->guardEntryNoticeList.size()];
}

std::string Config::ToString() const
{
    // return fmt::format("roomId: {}, danmuSeverConfUrl: {}", roomId,
    // danmuSeverConfUrl.ToString());
    return fmt::format(
        "roomId: {}, danmuSeverConfUrl: {} ,logLevel: {}, logPath: {}, danmuLength: {}, "
        "canPKNotice: {}, canGuardNotice: {}, canThanksGift: {}, canSuperChatNotice: {}, "
        "thanksGiftTimeout: {}, canDrawByLot: {}, canEntryNotice: {}, "
        "canThanksFocus: {}, canThanksShare: {}",
        this->roomId,
        this->danmuSeverConfUrl.ToString(),
        static_cast<uint8_t>(this->logLevel),
        this->logPath,
        this->danmuLength,
        this->canPKNotice,
        this->canGuardNotice,
        this->canThanksGift,
        this->canSuperChatNotice,
        this->thanksGiftTimeout,
        this->canDrawByLot,
        this->canEntryNotice,
        this->canThanksFocus,
        this->canThanksShare);
}

const std::string& Config::GetWbiMixKey() const
{
    return this->wbiMixKey;
}

uint64_t Config::GetRobotUID() const
{
    return this->robotUID;
}

Config::Config()
    : roomId{0}
    , danmuSeverConfUrl{"", 0, "", {}}
    , logLevel{LogLevel::Debug}
    , logPath{"log.txt"}
    , danmuLength{20}
    , canPKNotice{true}
    , canGuardNotice{true}
    , canThanksGift{true}
    , canSuperChatNotice{true}
    , thanksGiftTimeout{10}
    , canDrawByLot{true}
    , drawByLotList{}
    , canEntryNotice{true}
    , normalEntryNoticeList{}
    , guardEntryNoticeList{}
    , canThanksFocus{true}
    , canThanksShare{true}
    , robotUID{0}
    , wbiMixKey{}
    , uidUrl{"api.bilibili.com", 443, "/x/web-interface/nav"}
    , ctx(boost::asio::ssl::context::tlsv12_client)
    , resolver(ioc)
{
}