//
// Created by zeng on 24-6-10.
//

#ifndef LOGGER_H
#define LOGGER_H
#include "../../Base/noncopyable.h"

#include <fmt/format.h>
#include <fstream>
#include <string_view>

#ifndef MACOS
#    include <syncstream>
#else
#endif

// 定义日志级别
enum class LogLevel
{
    Fatal = 0,
    Error = 1,
    Warn  = 2,
    Info  = 3,
    Debug = 4,
    Test  = 5,
};

class Logger : noncopyable
{
public:
    ///@brief 获取单例对象
    static Logger* GetInstance();
    // ///@brief 设置日志级别
    // void set_log_level(LogLevel level);
    ///@brief 打印日志，使用std::format
    bool Log(LogLevel level, const std::string_view& file, int line, const std::string_view& func,
             const std::string_view& strMessage);
    ///@brief 设置日志级别
    void SetLogLevel(LogLevel level);
    ///@brief 获取日志级别
    [[nodiscard]] LogLevel GetLogLevel() const;
    ///@brief 设置日志保存地址
    void SetLogPath(const std::string& path);

private:
    constexpr inline std::string GetSign(LogLevel level);
    constexpr inline std::string GetSignWithColor(LogLevel level);
    Logger();
    ~Logger() = default;
    // 日志级别
    LogLevel logLevel;
    // 单例对象
    static Logger* pInstance;
    std::mutex     printMutex;
    std::ofstream  logFile;
    bool           isLogInFile;

#ifdef WIN32
    using DWORD = unsigned long;
    static DWORD pid;
#elif defined(__linux__) || defined(MACOS)
    static pid_t pid;
#endif

    // static std::osyncstream sync_out;   // std::cout 的同步包装
};

#define LOG_MESSAGE(level, message)                                                    \
    Logger::GetInstance()->GetLogLevel() >= level                                      \
        ? Logger::GetInstance()->Log(level, __FILE__, __LINE__, __FUNCTION__, message) \
        : false
#define LOG_VAR(level, var)                                                                    \
    Logger::GetInstance()->GetLogLevel() >= level                                              \
        ? Logger::GetInstance()->Log(                                                          \
              level, __FILE__, __LINE__, __FUNCTION__, fmt::format("{:>5}: {:>5}", #var, var)) \
        : false
#endif   // LOGGER_H
