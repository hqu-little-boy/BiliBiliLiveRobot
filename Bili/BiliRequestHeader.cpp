//
// Created by zeng on 24-7-2.
//

#include "BiliRequestHeader.h"

#include "../Base/Logger.h"
BiliRequestHeader* BiliRequestHeader::pInstance{nullptr};
BiliRequestHeader* BiliRequestHeader::GetInstance()
{
    if (pInstance == nullptr)
    {
        pInstance = new BiliRequestHeader();
    }
    return pInstance;
}

void BiliRequestHeader::DestroyInstance()
{
    if (pInstance != nullptr)
    {
        delete pInstance;
        pInstance = nullptr;
    }
}

const std::string& BiliRequestHeader::GetUserAgent() const
{
    return this->userAgent;
}

void BiliRequestHeader::SetUserAgent(const std::string& userAgent)
{
    this->userAgent = userAgent;
}

const BiliCookie& BiliRequestHeader::GetBiliCookie() const
{
    return this->biliCookie;
}

bool BiliRequestHeader::LoadBiliCookieByPath(const std::string_view& jsonPath)
{
    if (jsonPath.empty())
    {
        LOG_MESSAGE(LogLevel::ERROR, "jsonPath is empty");
        return false;
    }

    if (!std::filesystem::exists(jsonPath))
    {
        LOG_MESSAGE(LogLevel::ERROR, std::format("File not exists: {}", jsonPath));
        return false;
    }

    std::ifstream ifs(jsonPath.data());
    if (!ifs.is_open())
    {
        LOG_MESSAGE(LogLevel::ERROR, std::format("Failed to open file: {}", jsonPath));
        return false;
    }

    // 从文件中读取json
    nlohmann::json configJson;
    ifs >> configJson;
    return this->biliCookie.LoadBiliCookieByJson(configJson);
}

bool BiliRequestHeader::LoadBiliCookieByJson(const nlohmann::json& json)
{
    return this->biliCookie.LoadBiliCookieByJson(json);
}

BiliRequestHeader::BiliRequestHeader()
    : userAgent("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) "
                "Chrome/91.0.4472.124 Safari/537.36")
{
}