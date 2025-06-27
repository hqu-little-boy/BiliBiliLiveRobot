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
std::string Config::GetConfigPath()
{
    return "configure.json";
}
std::string Config::GetCookiePath()
{
    return "bili_cookie.json";
}

bool Config::LoadFromJson(std::string_view jsonPath)
{
    std::unique_lock<std::shared_mutex> lock(this->configMutex);
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
    this->configPath = jsonPath;
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
        if (!std::filesystem::exists("./Log"))
        {
            std::filesystem::create_directory("./Log");
        }
        this->logPath     = fmt::format("{}-{}", "./Log/log.txt", TimeStamp::Now().ToString());
        this->danmuLength = configJson["danmuLength"].get<uint8_t>();
        this->canPKNotice = configJson["canPKNotice"].get<bool>();
        this->canComeAroundNotice = configJson["canComeAroundNotice"].get<bool>();
        this->canGuardNotice      = configJson["canGuardNotice"].get<bool>();
        this->canThanksGift       = configJson["canThanksGift"].get<bool>();
        // this->giftThanksWord      = configJson["giftThanksWord"].get<std::string>();
        this->canThanksBlindBox  = configJson["canThanksBlindBox"].get<bool>();
        this->canSuperChatNotice = configJson["canSuperChatNotice"].get<bool>();
        this->thanksGiftTimeout  = configJson["thanksGiftTimeout"].get<uint8_t>();
        this->canDrawByLot       = configJson["canDrawByLot"].get<bool>();
        this->drawByLotList      = configJson["drawByLotList"].get<std::vector<std::string>>();
        this->canEntryNotice     = configJson["canEntryNotice"].get<bool>();
        this->normalEntryNoticeList =
            configJson["normalEntryNoticeList"].get<std::vector<std::string>>();
        this->guardEntryNoticeList =
            configJson["guardEntryNoticeList"].get<std::vector<std::string>>();

        this->canThanksFocus    = configJson["canThanksFocus"].get<bool>();
        this->thanksFocusNotice = configJson["thanksFocusNotice"].get<std::string>();
        this->canThanksShare    = configJson["canThanksShare"].get<bool>();
        this->thanksShareNotice = configJson["thanksShareNotice"].get<std::string>();
    }
    catch (const nlohmann::json::exception& e)
    {
        LOG_MESSAGE(LogLevel::Error, fmt::format("Failed to parse json: {}", e.what()));
        // throw std::runtime_error("Failed to parse json: " + std::string(e.what()));
        return false;
    }
    return true;
}
bool Config::SaveToJson()
{
    std::shared_lock<std::shared_mutex> lock(this->configMutex);
    if (this->configPath.empty())
    {
        LOG_MESSAGE(LogLevel::Error, "configPath is empty");
        throw std::runtime_error("configPath is empty");
        return false;
    }
    // if (!std::filesystem::exists(this->configPath))
    // {
    //     LOG_MESSAGE(LogLevel::Error, fmt::format("File not exists: {}", this->configPath));
    //     throw std::runtime_error(fmt::format("File not exists: {}", this->configPath));
    //     return false;
    // }
    nlohmann::json configJson;
    configJson["roomId"]   = this->roomId;
    configJson["logLevel"] = static_cast<unsigned>(this->logLevel);
    // configJson["logPath"]             = this->logPath;
    configJson["danmuLength"]         = this->danmuLength;
    configJson["canPKNotice"]         = this->canPKNotice;
    configJson["canComeAroundNotice"] = this->canComeAroundNotice;
    configJson["canGuardNotice"]      = this->canGuardNotice;
    configJson["canThanksGift"]       = this->canThanksGift;
    // configJson["giftThanksWord"]        = this->giftThanksWord;
    configJson["canThanksBlindBox"]     = this->canThanksBlindBox;
    configJson["canSuperChatNotice"]    = this->canSuperChatNotice;
    configJson["thanksGiftTimeout"]     = this->thanksGiftTimeout;
    configJson["canDrawByLot"]          = this->canDrawByLot;
    configJson["drawByLotList"]         = this->drawByLotList;
    configJson["canEntryNotice"]        = this->canEntryNotice;
    configJson["normalEntryNoticeList"] = this->normalEntryNoticeList;
    configJson["guardEntryNoticeList"]  = this->guardEntryNoticeList;
    configJson["canThanksFocus"]        = this->canThanksFocus;
    configJson["thanksFocusNotice"]     = this->thanksFocusNotice;
    configJson["canThanksShare"]        = this->canThanksShare;
    configJson["thanksShareNotice"]     = this->thanksShareNotice;

    std::ofstream ofs(this->configPath.data());
    if (!ofs.is_open())
    {
        LOG_MESSAGE(LogLevel::Error, fmt::format("Failed to open file: {}", this->configPath));
        throw std::runtime_error(fmt::format("Failed to open file: {}", this->configPath));
        return false;
    }
    ofs << std::setw(4) << configJson << std::endl;   // 格式化输出
    LOG_MESSAGE(LogLevel::Info, fmt::format("Config saved to: {}", this->configPath));
    return true;
}
bool Config::LoadUID()
{
    std::unique_lock<std::shared_mutex>                lock(this->configMutex);
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
    boost::system::error_code ecResolver;
    const auto                results{this->resolver.resolve(
        this->uidUrl.GetHost(), std::to_string(this->uidUrl.GetPort()), ecResolver)};
    if (ecResolver)
    {
        LOG_VAR(
            LogLevel::Error,
            fmt::format("Failed to resolve {}: {}", this->uidUrl.GetHost(), ecResolver.message()));
        return false;
    }
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
        boost::beast::http::verb::get, this->uidUrl.GetTarget(), 11};
    req.set(boost::beast::http::field::host, this->uidUrl.GetHost());
    req.set(boost::beast::http::field::user_agent,
            BiliRequestHeader::GetInstance()->GetUserAgent());
    // req.set(boost::beast::http::field::content_type, "application/json");
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
    boost::system::error_code                                     ecRead;
    boost::beast::http::read(stream, buffer, res, ecRead);
    if (ecRead)
    {
        LOG_VAR(LogLevel::Error, fmt::format("Failed to read response: {}", ecRead.message()));
        return false;
    }
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
    this->robotUID   = roomInfoJson["data"]["mid"].get<uint64_t>();
    this->robotUname = roomInfoJson["data"]["uname"].get<std::string>();
    this->isLogined  = roomInfoJson["data"]["isLogin"].get<bool>();
    this->robotFace  = roomInfoJson["data"]["face"].get<std::string>();
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
    LOG_MESSAGE(LogLevel::Info, fmt::format("WBI Mix Key: {}", this->wbiMixKey));
    return true;
}

uint64_t Config::GetRoomId() const
{
    std::shared_lock<std::shared_mutex> lock(this->configMutex);
    return this->roomId;
}
void Config::SetRoomId(uint64_t roomId)
{
    std::unique_lock<std::shared_mutex> lock(this->configMutex);
    this->roomId = roomId;
    // 更新弹幕服务器配置URL
    this->danmuSeverConfUrl = Url("api.live.bilibili.com",
                                  443,
                                  "/xlive/web-room/v1/index/getDanmuInfo",
                                  {{"id", std::to_string(this->roomId)}, {"type", "0"}});
}

const Url& Config::GetDanmuSeverConfUrl() const
{
    std::shared_lock<std::shared_mutex> lock(this->configMutex);
    return this->danmuSeverConfUrl;
}

LogLevel Config::GetLogLevel() const
{
    std::shared_lock<std::shared_mutex> lock(this->configMutex);
    return this->logLevel;
}

const std::string& Config::GetLogPath() const
{
    std::shared_lock<std::shared_mutex> lock(this->configMutex);
    return this->logPath;
}

uint8_t Config::GetDanmuLength() const
{
    std::shared_lock<std::shared_mutex> lock(this->configMutex);
    return this->danmuLength;
}

bool Config::CanPKNotice() const
{
    std::shared_lock<std::shared_mutex> lock(this->configMutex);
    return this->canPKNotice;
}
void Config::SetCanPKNotice(bool canPK)
{
    std::unique_lock<std::shared_mutex> lock(this->configMutex);
    this->canPKNotice = canPK;
}
bool Config::CanComeAroundNotice() const
{
    std::shared_lock<std::shared_mutex> lock(this->configMutex);
    return this->canComeAroundNotice;
}
void Config::SetCanComeAroundNotice(bool canComeAroundNotice)
{
    std::unique_lock<std::shared_mutex> lock(this->configMutex);
    this->canComeAroundNotice = canComeAroundNotice;
}
bool Config::CanGuardNotice() const
{
    std::shared_lock<std::shared_mutex> lock(this->configMutex);
    return this->canGuardNotice;
}
void Config::SetCanGuardNotice(bool canGuard)
{
    std::unique_lock<std::shared_mutex> lock(this->configMutex);
    this->canGuardNotice = canGuard;
}

bool Config::CanThanksGift() const
{
    std::shared_lock<std::shared_mutex> lock(this->configMutex);
    return this->canThanksGift;
}
void Config::SetCanThanksGift(bool canThanksGift)
{
    std::unique_lock<std::shared_mutex> lock(this->configMutex);
    this->canThanksGift = canThanksGift;
}

bool Config::CanSuperChatNotice() const
{
    std::shared_lock<std::shared_mutex> lock(this->configMutex);
    return this->canSuperChatNotice;
}
void Config::SetCanSuperChatNotice(bool canSuperChat)
{
    std::unique_lock<std::shared_mutex> lock(this->configMutex);
    this->canSuperChatNotice = canSuperChat;
}

uint8_t Config::GetThanksGiftTimeout() const
{
    std::shared_lock<std::shared_mutex> lock(this->configMutex);
    return this->thanksGiftTimeout;
}
void Config::SetThanksGiftTimeout(uint8_t timeout)
{
    std::unique_lock<std::shared_mutex> lock(this->configMutex);
    if (timeout < 0)
    {
        LOG_MESSAGE(LogLevel::Warn, "Thanks gift timeout must be greater than or equal to 0");
        return;
    }
    this->thanksGiftTimeout = timeout;
}
// const std::string& Config::GetGiftThanksWord() const
// {
//     std::shared_lock<std::shared_mutex> lock(this->configMutex);
//     return this->giftThanksWord;
// }
// void Config::SetGiftThanksWord(const std::string& word)
// {
//     std::unique_lock<std::shared_mutex> lock(this->configMutex);
//     if (word.empty())
//     {
//         LOG_MESSAGE(LogLevel::Warn, "Gift thanks word cannot be empty");
//         return;
//     }
//     this->giftThanksWord = word;
// }
bool Config::CanThanksBlindBox() const
{
    std::shared_lock<std::shared_mutex> lock(this->configMutex);
    return this->canThanksBlindBox;
}
void Config::SetCanThanksBlindBox(bool canThanksBlindBox)
{
    std::unique_lock<std::shared_mutex> lock(this->configMutex);
    this->canThanksBlindBox = canThanksBlindBox;
}

bool Config::CanDrawByLot() const
{
    std::shared_lock<std::shared_mutex> lock(this->configMutex);
    return this->canDrawByLot;
}
void Config::SetCanDrawByLot(bool canDrawByLot)
{
    std::unique_lock<std::shared_mutex> lock(this->configMutex);
    this->canDrawByLot = canDrawByLot;
}

const std::string& Config::GetDrawByLotWord() const
{
    std::shared_lock<std::shared_mutex> lock(this->configMutex);
    std::random_device                  seed_gen;
    std::seed_seq seed_sequence{seed_gen(), seed_gen(), seed_gen(), seed_gen()};
    std::mt19937  engine(seed_sequence);
    return this->drawByLotList[engine() % this->drawByLotList.size()];
}
std::vector<std::string>& Config::GetDrawByLotWordList()
{
    std::shared_lock<std::shared_mutex> lock(this->configMutex);
    return this->drawByLotList;
}

bool Config::CanThanksFocus() const
{
    std::shared_lock<std::shared_mutex> lock(this->configMutex);
    return this->canThanksFocus;
}

void Config::SetCanThanksFocus(bool canThanksFocus)
{
    std::shared_lock<std::shared_mutex> lock(this->configMutex);
    this->canThanksFocus = canThanksFocus;
}
const std::string& Config::GetThanksFocusWord() const
{
    std::shared_lock<std::shared_mutex> lock(this->configMutex);
    return this->thanksFocusNotice;
}
void Config::SetThanksFocusWord(std::string_view thanksFocusWord)
{
    std::unique_lock<std::shared_mutex> lock(this->configMutex);
    this->thanksFocusNotice = thanksFocusWord;
}

bool Config::CanThanksShare() const
{
    std::shared_lock<std::shared_mutex> lock(this->configMutex);
    return this->canThanksShare;
}

void Config::SetCanThanksShare(bool canThanksShare)
{
    std::shared_lock<std::shared_mutex> lock(this->configMutex);
    this->canThanksShare = canThanksShare;
}
const std::string& Config::GetThanksShareWord() const
{
    std::shared_lock<std::shared_mutex> lock(this->configMutex);
    return this->thanksShareNotice;
}
void Config::SetThanksShareWord(std::string_view thanksShareWord)
{
    std::unique_lock<std::shared_mutex> lock(this->configMutex);
    this->thanksShareNotice = thanksShareWord;
}

bool Config::CanEntryNotice() const
{
    std::shared_lock<std::shared_mutex> lock(this->configMutex);
    return this->canEntryNotice;
}

void Config::SetCanEntryNotice(bool canEntryNotice)
{
    std::shared_lock<std::shared_mutex> lock(this->configMutex);
    this->canEntryNotice = canEntryNotice;
}

const std::string& Config::GetNormalEntryNoticeWord() const
{
    std::shared_lock<std::shared_mutex> lock(this->configMutex);
    std::random_device                  seed_gen;
    std::seed_seq seed_sequence{seed_gen(), seed_gen(), seed_gen(), seed_gen()};
    std::mt19937  engine(seed_sequence);
    return this->normalEntryNoticeList[engine() % this->normalEntryNoticeList.size()];
}
std::vector<std::string>& Config::GetNormalEntryNoticeList()
{
    std::shared_lock<std::shared_mutex> lock(this->configMutex);
    return this->normalEntryNoticeList;
}
std::vector<std::string>& Config::GetGuardEntryNoticeList()
{
    std::shared_lock<std::shared_mutex> lock(this->configMutex);
    return this->guardEntryNoticeList;
}

const std::string& Config::GetGuardEntryNoticeWord() const
{
    std::shared_lock<std::shared_mutex> lock(this->configMutex);
    std::random_device                  seed_gen;
    std::seed_seq seed_sequence{seed_gen(), seed_gen(), seed_gen(), seed_gen()};
    std::mt19937  engine(seed_sequence);
    return this->guardEntryNoticeList[engine() % this->guardEntryNoticeList.size()];
}

std::string Config::ToString() const
{
    std::shared_lock<std::shared_mutex> lock(this->configMutex);
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
    std::shared_lock<std::shared_mutex> lock(this->configMutex);
    return this->wbiMixKey;
}

uint64_t Config::GetRobotUID() const
{
    std::shared_lock<std::shared_mutex> lock(this->configMutex);
    return this->robotUID;
}
const std::string& Config::GetRobotUname() const
{
    std::shared_lock<std::shared_mutex> lock(this->configMutex);
    return this->robotUname;
}
const std::string& Config::GetRobotFace() const
{
    std::shared_lock<std::shared_mutex> lock(this->configMutex);
    return this->robotFace;
}
bool Config::IsLogined() const
{
    std::shared_lock<std::shared_mutex> lock(this->configMutex);
    return this->isLogined;
}
std::shared_lock<std::shared_mutex> Config::GetSharedLock()
{
    return std::shared_lock<std::shared_mutex>(configMutex);
}

std::unique_lock<std::shared_mutex> Config::GetUniqueLock()
{
    return std::unique_lock<std::shared_mutex>(configMutex);
}
Config::Config()
    : roomId{0}
    , danmuSeverConfUrl{"", 0, "", {}}
    , logLevel{LogLevel::Debug}
    , logPath{"./Log/log.txt"}
    , danmuLength{20}
    , canPKNotice{false}
    , canComeAroundNotice(false)
    , canGuardNotice{false}
    , canThanksGift{false}
    , canThanksBlindBox(false)
    , canSuperChatNotice{false}
    , thanksGiftTimeout{10}
    , canDrawByLot{false}
    , drawByLotList{}
    , canEntryNotice{false}
    , normalEntryNoticeList{}
    , guardEntryNoticeList{}
    , canThanksFocus{false}
    , canThanksShare{false}
    , wbiMixKey{}
    , uidUrl{"api.bilibili.com", 443, "/x/web-interface/nav"}
    , robotUID{0}
    , robotUname{}
    , isLogined{false}
    , robotFace{}
    , ctx(boost::asio::ssl::context::tlsv12_client)
    , resolver(ioc)
    , configPath{}
    , configMutex{}
{
}