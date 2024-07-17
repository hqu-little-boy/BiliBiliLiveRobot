//
// Created by zeng on 24-6-10.
//

#include "Logger.h"

#include "TimeStamp.h"

#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>
#ifdef WIN32
#    include <windows.h>
// 进程ID
DWORD Logger::pid = GetCurrentProcessId();
#elif defined(__linux__)
#    include <unistd.h>
// 进程ID
pid_t Logger::pid = getpid();
#endif

Logger* Logger::pInstance = new Logger();
// std::osyncstream Logger::sync_out  = std::osyncstream{std::cout};


Logger* Logger::GetInstance()
{
    // if (pInstance == nullptr)
    // {
    //     pInstance = new Logger();
    // }
    assert(pInstance != nullptr);
    return pInstance;
}

// void Logger::set_log_level(LogLevel level)
// {
//     log_level_ = level;
// }

bool Logger::Log(LogLevel level, const std::string_view& file, int line,
                 const std::string_view& func, const std::string_view& strMessage)
{
    std::string strTime = TimeStamp::Now().ToString();
    // 线程ID
    std::thread::id tid = std::this_thread::get_id();

    // 日志内容，包括时间、进程ID、线程ID、文件、行号、函数名、日志级别、日志内容，固定长度和对齐
    // std::string strLog = std::format("{} {} {} {} {} {} {} {}\n", strTime, pid, tid, file,
    // line, func, strLevel,
    //                                  strMessage);
    std::string strLog = std::format(
        "{:<19} {:<5} {:<5} {:<5} {:<5} {:<5} {}", strTime, pid, tid, file, func, line, strMessage);
    // 互斥锁，保证输出到标准输出的内容不会混乱
    std::lock_guard<std::mutex> guard(printMutex);
    // 输出到标准输出
    // std::println(std::cout, "{}", strLog);
    std::cout << std::format("[{:<7}] {}", this->GetSignWithColor(level), strLog) << std::endl;
    if (isLogInFile)
    {
        this->logFile << std::format("[{:<7}] {}", this->GetSign(level), strLog) << std::endl;
        // std::println(this->logFile, "{}", strLog);
    }
    // Logger::sync_out << strLog << std::endl;
    return true;
}

void Logger::SetLogLevel(LogLevel level)
{
    logLevel = level;
}

LogLevel Logger::GetLogLevel() const
{
    return logLevel;
}

void Logger::SetLogPath(const std::string& path)
{
    if (path.empty())
    {
        isLogInFile = false;
        return;
    }
    logFile.open(path);
    isLogInFile = true;
    if (!logFile.is_open())
    {
        std::cerr << "Open log file failed!" << std::endl;
    }
}

constexpr std::string Logger::GetSign(LogLevel level)
{
    switch (level)
    {
    case LogLevel::Info:
    {
        return "INFO";
        break;
    }
    case LogLevel::Warn:
    {
        return "WARN";
        break;
    }
    case LogLevel::Error:
    {
        return "ERROR";
        break;
    }
    case LogLevel::Fatal:
    {
        return "FATAL";
        break;
    }
    case LogLevel::Debug:
    {
        return "DEBUG";
        break;
    }
    case LogLevel::Test:
    {
        return "TEST";
        break;
    }
    default:
    {
        break;
    }
    }
    LOG_MESSAGE(LogLevel::Error, "Invalid log level");
    return "";
}

constexpr std::string Logger::GetSignWithColor(LogLevel level)
{
    switch (level)
    {
    case LogLevel::Info:
    {
        return "\033[32mINFO\033[0m";
        break;
    }
    case LogLevel::Warn:
    {
        return "\033[33mWARN\033[0m";
        break;
    }
    case LogLevel::Error:
    {
        return "\033[31mERROR\033[0m";
        break;
    }
    case LogLevel::Fatal:
    {
        return "\033[31mFATAL\033[0m";
        break;
    }
    case LogLevel::Debug:
    {
        return "\033[37mDEBUG\033[0m";
        break;
    }
    case LogLevel::Test:
    {
        return "\033[36mTEST\033[0m";
        break;
    }
    default:
    {
        break;
    }
    }
    LOG_MESSAGE(LogLevel::Error, "Invalid log level");
    return "";
}

Logger::Logger()
    : logLevel(LogLevel::Debug)
    , isLogInFile(false)
{
}
