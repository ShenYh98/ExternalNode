#pragma once

#include <iostream>
#include <string>
#include <fstream>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

#define LOG_INFO(msg, ...)                  \
    {                                  \
        COMMONLOG_INSTANCE.fileLogger->info(msg, ##__VA_ARGS__);    \
        COMMONLOG_INSTANCE.fileLogger->flush();                     \
    }
#define LOG_ERROR(msg, ...)                 \
    {                                  \
        COMMONLOG_INSTANCE.fileLogger->error(msg, ##__VA_ARGS__);    \
        COMMONLOG_INSTANCE.fileLogger->flush();                     \
    }

#define LOG_WARN(msg, ...)                  \
    {                                  \
        COMMONLOG_INSTANCE.fileLogger->warn(msg, ##__VA_ARGS__);    \
        COMMONLOG_INSTANCE.fileLogger->flush();                     \
    }

#define LOG_TRACE(msg, ...)                 \
    {                                  \
        COMMONLOG_INSTANCE.fileLogger->trace(msg, ##__VA_ARGS__);    \
        COMMONLOG_INSTANCE.fileLogger->flush();                     \
    }

#define LOG_DEBUG(msg, ...)                 \
    {                                  \
        COMMONLOG_INSTANCE.fileLogger->debug(msg, ##__VA_ARGS__);    \
        COMMONLOG_INSTANCE.fileLogger->flush();                     \
    }

#define LOG_CRITICAL(msg, ...)              \
    {                                  \
        COMMONLOG_INSTANCE.fileLogger->critical(msg, ##__VA_ARGS__);    \
        COMMONLOG_INSTANCE.fileLogger->flush();                     \
    }

// 宏定义在外部调用静态成员函数来创建实例
#define COMMONLOG_INIT(path)               \
    class Initializer {                     \
    public:                                 \
        Initializer() {                     \
            CommonLib::CommonLog::getInstance().initLogParam(path); \
        }                                   \
    };                                      \
    static Initializer initializer

namespace CommonLib {

#define COMMONLOG_INSTANCE CommonLib::CommonLog::getInstance()

class CommonLog {
public:
    CommonLog(const CommonLog&) = delete;
    CommonLog& operator = (const CommonLog&) = delete;

    static CommonLog& getInstance();
    void initLogParam(const std::string& path);
    std::string getFileName(const std::string& fullPath);

private:
    CommonLog();
    ~CommonLog();

    static CommonLog& createInstance();

    int getLogParamByJson(const std::string& path);
    void setLogParam();

private:
    int log_size;
    int log_level;
    int log_num;
    std::string log_path;
    
    friend class Initializer;

public:
    std::shared_ptr<spdlog::logger> fileLogger;
};

}


