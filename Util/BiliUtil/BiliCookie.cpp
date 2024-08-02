//
// Created by zeng on 24-7-2.
//

#include "BiliCookie.h"

#include "../../Entity/Global/Logger.h"

bool BiliCookie::LoadBiliCookieByJson(const nlohmann::json& json)
{
    try
    {
        this->buvid3      = json["buvid3"].get<std::string>();
        this->buvid4      = json["buvid4"].get<std::string>();
        this->strSESSDATA = json["SESSDATA"].get<std::string>();
        this->bili_jct    = json["bili_jct"].get<std::string>();
        this->sid         = json["sid"].get<std::string>();
        // 字符串转换为数字
        this->strDedeUserID        = std::stoull(json["DedeUserID"].get<std::string>());
        this->strDedeUserID__ckMd5 = json["DedeUserID__ckMd5"].get<std::string>();
        return true;
    }
    catch (const nlohmann::json::exception& e)
    {
        LOG_MESSAGE(LogLevel::Error, FORMAT("Failed to load BiliCookie: {}", e.what()));
        return false;
    }
}

// bool BiliCookie::LoadBiliCookieByPath(const std::string& jsonPath)
// {
//     if (jsonPath.empty())
//     {
//         LOG_MESSAGE(LogLevel::Error, "jsonPath is empty");
//         return false;
//     }
//
//     if (!std::filesystem::exists(jsonPath))
//     {
//         LOG_MESSAGE(LogLevel::Error, FORMAT("File not exists: {}", jsonPath));
//         return false;
//     }
//
//     std::ifstream ifs(jsonPath);
//     if (!ifs.is_open())
//     {
//         LOG_MESSAGE(LogLevel::Error, FORMAT("Failed to open file: {}", jsonPath));
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

const std::string& BiliCookie::GetBuvid4() const
{
    return this->buvid4;
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

std::string BiliCookie::ToString() const
{
    return FORMAT("SESSDATA={}; buvid3={}; buvid4={}; bili_jct={}; sid={}; DedeUserID={}; "
                  "DedeUserID__ckMd5={}",
                  this->strSESSDATA,
                  this->buvid3,
                  this->buvid4,
                  this->bili_jct,
                  this->sid,
                  this->strDedeUserID,
                  this->strDedeUserID__ckMd5);
}