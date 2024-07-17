//
// Created by zeng on 24-6-26.
//

#include "Config.h"

#include "Logger.h"
#include "nlohmann/json.hpp"

#include <fstream>
#include <random>
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

bool Config::LoadFromJson(const std::string& jsonPath)
{
    if (jsonPath.empty())
    {
        LOG_MESSAGE(LogLevel::Error, "jsonPath is empty");
        throw std::runtime_error("jsonPath is empty");
        return false;
    }

    if (!std::filesystem::exists(jsonPath))
    {
        LOG_MESSAGE(LogLevel::Error, std::format("File not exists: {}", jsonPath));
        throw std::runtime_error("File not exists: " + jsonPath);
        return false;
    }

    std::ifstream ifs(jsonPath);
    if (!ifs.is_open())
    {
        LOG_MESSAGE(LogLevel::Error, std::format("Failed to open file: {}", jsonPath));
        throw std::runtime_error("Failed to open file: " + jsonPath);
        return false;
    }

    try
    {
        // 从文件中读取json
        nlohmann::json configJson;
        ifs >> configJson;
        this->roomId             = configJson["roomId"].get<uint64_t>();
        int logLevelInt          = configJson["logLevel"].get<unsigned>();
        this->logLevel           = static_cast<LogLevel>(logLevelInt);
        this->danmuSeverConfUrl  = Url("api.live.bilibili.com",
                                      443,
                                      "/xlive/web-room/v1/index/getDanmuInfo",
                                       {{"id", std::to_string(this->roomId)}, {"type", "0"}});
        this->logPath            = configJson["logPath"].get<std::string>();
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
        LOG_MESSAGE(LogLevel::Error, std::format("Failed to parse json: {}", e.what()));
        // throw std::runtime_error("Failed to parse json: " + std::string(e.what()));
        return false;
    }
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
    // return std::format("roomId: {}, danmuSeverConfUrl: {}", roomId,
    // danmuSeverConfUrl.ToString());
    return std::format(
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
{
}