//
// Created by zeng on 24-6-26.
//

#ifndef CONFIG_H
#define CONFIG_H
#include "../Base/Logger.h"
#include "Url.h"


/// @brief 配置类，单例模式
class Config
{
public:
    static Config* GetInstance();
    static void    DestroyInstance();

    bool                     LoadFromJson(const std::string& jsonPath);
    [[nodiscard]] unsigned   GetRoomId() const;
    [[nodiscard]] const Url& GetDanmuSeverConfUrl() const;
    [[nodiscard]] LogLevel   GetLogLevel() const;
    [[nodiscard]] std::string ToString() const;

private:
    Config()                         = default;
    ~Config()                        = default;
    Config(const Config&)            = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&)                 = delete;
    Config& operator=(Config&&)      = delete;

private:
    static Config* instance;

    unsigned roomId;
    Url      danmuSeverConfUrl;
    LogLevel logLevel;
};

#endif   // CONFIG_H
