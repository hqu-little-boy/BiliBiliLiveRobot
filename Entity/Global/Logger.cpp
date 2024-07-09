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
    if (level > logLevel)
    {
        return false;
    }
    std::string strLevel;
    switch (level)
    {
    case LogLevel::Info:
    {
        strLevel = "\033[32mINFO\033[0m";
        break;
    }
    case LogLevel::Warn:
    {
        strLevel = "\033[33mWARN\033[0m";
        break;
    }
    case LogLevel::Error:
    {
        strLevel = "\033[31mERROR\033[0m";
        break;
    }
    case LogLevel::Fatal:
    {
        strLevel = "\033[31mFATAL\033[0m";
        break;
    }
    case LogLevel::Debug:
    {
        strLevel = "\033[37mDEBUG\033[0m";
        break;
    }
    default:
    {
        break;
    }
    }

    std::string strTime = TimeStamp::Now().ToString();
    // 线程ID
    std::thread::id tid = std::this_thread::get_id();

    // 日志内容，包括时间、进程ID、线程ID、文件、行号、函数名、日志级别、日志内容，固定长度和对齐
    // std::string strLog = std::format("{} {} {} {} {} {} {} {}\n", strTime, pid, tid, file,
    // line, func, strLevel,
    //                                  strMessage);
    std::string strLog = std::format("[{:<7}] {:<19} {:<5} {:<5} {:<5} {:<5} {:<5} {}",
                                     strLevel,
                                     strTime,
                                     pid,
                                     tid,
                                     file,
                                     func,
                                     line,
                                     strMessage);
    // 互斥锁，保证输出到标准输出的内容不会混乱
    std::lock_guard<std::mutex> guard(printMutex);
    // 输出到标准输出
    // std::println(std::cout, "{}", strLog);
    std::cout << strLog << std::endl;
    if (isLogInFile)
    {
        this->logFile << strLog << std::endl;
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

Logger::Logger()
    : logLevel(LogLevel::Debug)
    , isLogInFile(false)
{
}
