//
// Created by zeng on 24-6-10.
//

#include "Logger.h"

#include "TimeStamp.h"

#include <chrono>
#include <iostream>
#include <thread>
#include <unistd.h>

Logger* Logger::pInstance = nullptr;
// std::osyncstream Logger::sync_out  = std::osyncstream{std::cout};


Logger* Logger::GetInstance()
{
    if (pInstance == nullptr)
    {
        pInstance = new Logger();
    }
    return pInstance;
}

// void Logger::set_log_level(LogLevel level)
// {
//     log_level_ = level;
// }

void Logger::Log(LogLevel level, std::string_view file, int line, std::string_view func,
                 std::string_view strMessage)
{
    if (level > logLevel)
    {
        return;
    }
    std::string strLevel;
    switch (level)
    {
    case LogLevel::INFO:
    {
        strLevel = "INFO";
        break;
    }
    case LogLevel::WARN:
    {
        strLevel = "WARN";
        break;
    }
    case LogLevel::ERROR:
    {
        strLevel = "ERROR";
        break;
    }
    case LogLevel::FATAL:
    {
        strLevel = "FATAL";
        break;
    }
    case LogLevel::DEBUG:
    {
        strLevel = "DEBUG";
        break;
    }
    default:
    {
        break;
    }
    }

    std::string strTime = TimeStamp::Now().ToString();
    // 进程ID
    pid_t pid = getpid();
    // 线程ID
    std::thread::id tid = std::this_thread::get_id();

    // 日志内容，包括时间、进程ID、线程ID、文件、行号、函数名、日志级别、日志内容，固定长度和对齐
    // std::string strLog = std::format("{} {} {} {} {} {} {} {}\n", strTime, pid, tid, file,
    // line, func, strLevel,
    //                                  strMessage);
    std::string strLog = std::format("[{:<5}] {:<19} {:<5} {:<5} {:<5} {:<5} {:<5} {}",
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
    std::println(std::cout, "{}", strLog);
    // Logger::sync_out << strLog << std::endl;
}

void Logger::SetLogLevel(LogLevel level)
{
    logLevel = level;
}

Logger::Logger()
    : logLevel(LogLevel::DEBUG)
{
}
