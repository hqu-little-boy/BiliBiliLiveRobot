//
// Created by zeng on 24-6-26.
//

#include "Config.h"

#include "Logger.h"
#include "nlohmann/json.hpp"

#include <fstream>
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
        this->roomId       = configJson["roomId"].get<unsigned>();
        int logLevelInt    = configJson["logLevel"].get<unsigned>();
        this->logLevel     = static_cast<LogLevel>(logLevelInt);
        std::string host   = configJson["danmuSeverConfUrl"]["host"].get<std::string>();
        unsigned    port   = configJson["danmuSeverConfUrl"]["port"].get<unsigned>();
        std::string target = configJson["danmuSeverConfUrl"]["target"].get<std::string>();
        std::list<std::pair<std::string, std::string>> query;
        query.emplace_back(std::string("id"), std::to_string(this->roomId));
        for (const auto& [key, value] : configJson["danmuSeverConfUrl"]["query"].items())
        {
            // query[key] = value.get<std::string>();
            query.emplace_back(key, value.get<std::string>());
        }
        this->danmuSeverConfUrl = Url(host, port, target, query);
        this->logPath           = configJson["logPath"].get<std::string>();
    }
    catch (const nlohmann::json::exception& e)
    {
        LOG_MESSAGE(LogLevel::Error, std::format("Failed to parse json: {}", e.what()));
        // throw std::runtime_error("Failed to parse json: " + std::string(e.what()));
        return false;
    }
    return true;
}

unsigned Config::GetRoomId() const
{
    return roomId;
}

const Url& Config::GetDanmuSeverConfUrl() const
{
    return danmuSeverConfUrl;
}

LogLevel Config::GetLogLevel() const
{
    return logLevel;
}

const std::string& Config::GetLogPath() const
{
    return logPath;
}

std::string Config::ToString() const
{
    return std::format("roomId: {}, danmuSeverConfUrl: {}", roomId, danmuSeverConfUrl.ToString());
}