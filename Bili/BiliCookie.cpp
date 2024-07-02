//
// Created by zeng on 24-7-2.
//

#include "BiliCookie.h"

#include "../Base/Logger.h"

bool BiliCookie::LoadBiliCookie(const nlohmann::json& json)
{
    try
    {
        this->buvid3               = json.at("buvid3").get<std::string>();
        this->b_nut                = json.at("b_nut").get<std::string>();
        this->strSESSDATA          = json.at("SESSDATA").get<std::string>();
        this->bili_jct             = json.at("bili_jct").get<std::string>();
        this->sid                  = json.at("sid").get<std::string>();
        this->strDedeUserID        = json.at("DedeUserID").get<uint64_t>();
        this->strDedeUserID__ckMd5 = json.at("DedeUserID__ckMd5").get<std::string>();
        return true;
    }
    catch (const nlohmann::json::exception& e)
    {
        LOG_MESSAGE(LogLevel::ERROR, std::format("Failed to load BiliCookie: {}", e.what()));
        return false;
    }
}

// bool BiliCookie::LoadBiliCookie(const std::string& jsonPath)
// {
//     if (jsonPath.empty())
//     {
//         LOG_MESSAGE(LogLevel::ERROR, "jsonPath is empty");
//         return false;
//     }
//
//     if (!std::filesystem::exists(jsonPath))
//     {
//         LOG_MESSAGE(LogLevel::ERROR, std::format("File not exists: {}", jsonPath));
//         return false;
//     }
//
//     std::ifstream ifs(jsonPath);
//     if (!ifs.is_open())
//     {
//         LOG_MESSAGE(LogLevel::ERROR, std::format("Failed to open file: {}", jsonPath));
//         return false;
//     }
//
//     // 从文件中读取json
//     nlohmann::json configJson;
//     ifs >> configJson;
//     return this->LoadBiliCookie(configJson);
// }

const std::string& BiliCookie::GetBuvid3() const
{
    return this->buvid3;
}

const std::string& BiliCookie::GetBNut() const
{
    return this->b_nut;
}

const std::string& BiliCookie::GetSESSDATA() const
{
    return this->strSESSDATA;
}

const std::string& BiliCookie::GetBiliJct() const
{
    return this->bili_jct;
}

const std::string& BiliCookie::GetSid() const
{
    return this->sid;
}

uint64_t BiliCookie::GetDedeUserID() const
{
    return this->strDedeUserID;
}

const std::string& BiliCookie::GetDedeUserIDCkMd5() const
{
    return this->strDedeUserID__ckMd5;
}