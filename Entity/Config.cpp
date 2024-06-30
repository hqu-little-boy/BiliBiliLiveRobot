//
// Created by zeng on 24-6-26.
//

#include "Config.h"

#include "../Base/Logger.h"
#include "nlohmann/json.hpp"

#include <fstream>
Config* Config::instance{new Config()};
Config* Config::GetInstance()
{
    // if (instance == nullptr)
    // {
    //     instance = new Config();
    // }
    assert(instance != nullptr);
    return instance;
}

void Config::DestroyInstance()
{
    if (instance != nullptr)
    {
        delete instance;
        instance = nullptr;
    }
}

bool Config::LoadFromJson(const std::string& jsonPath)
{
    std::ifstream ifs(jsonPath);
    if (!ifs.is_open())
    {
        LOG_MESSAGE(LogLevel::ERROR, std::format("Failed to open file: {}", jsonPath));
        throw std::runtime_error("Failed to open file: " + jsonPath);
        return false;
    }

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

std::string Config::ToString() const
{
    return std::format("roomId: {}, danmuSeverConfUrl: {}", roomId, danmuSeverConfUrl.ToString());
}