#pragma once
#include <string>

class Logger
{
public:
    enum Level
    {
        INFO,
        WARNING,
        ERROR
    };

    // 静态调用接口
    static void info(const std::string &msg);
    static void warn(const std::string &msg);
    static void error(const std::string &msg);

private:
    // 核心记录逻辑
    static void log(Level level, const std::string &message);

    // 获取并格式化当前时间字符串
    static std::string get_current_time();
};