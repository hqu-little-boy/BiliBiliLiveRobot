//
// Created by zeng on 24-6-10.
//

#ifndef LOGGER_H
#define LOGGER_H

// 定义日志级别
enum class LogLevel
{
    FATAL = 0,
    ERROR = 1,
    WARN  = 2,
    INFO  = 3,
    DEBUG = 4,
};

#include "noncopyable.h"

#include <format>
#include <string_view>
#include <syncstream>

class Logger : noncopyable
{
public:
    ///@brief 获取单例对象
    static Logger* GetInstance();
    // ///@brief 设置日志级别
    // void set_log_level(LogLevel level);
    ///@brief 打印日志，使用std::format
    void Log(LogLevel level, std::string_view file, int line, std::string_view func,
             std::string_view strMessage);
    ///@brief 设置日志级别
    void SetLogLevel(LogLevel level);

private:
    Logger();
    ~Logger() = default;
    // 日志级别
    LogLevel logLevel;
    // 单例对象
    static Logger* pInstance;
    std::mutex     printMutex;

    // static std::osyncstream sync_out;   // std::cout 的同步包装
};

#define LOG_MESSAGE(level, message) \
    Logger::GetInstance()->Log(level, __FILE__, __LINE__, __FUNCTION__, message)
#define LOG_VAR(level, var)     \
    Logger::GetInstance()->Log( \
        level, __FILE__, __LINE__, __FUNCTION__, std::format("{:>5}: {:>5}", #var, var))
#endif   // LOGGER_H
