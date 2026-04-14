#include "logger.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

void Logger::info(const std::string &msg)
{
    log(INFO, msg);
}

void Logger::warn(const std::string &msg)
{
    log(WARNING, msg);
}

void Logger::error(const std::string &msg)
{
    log(ERROR, msg);
}

void Logger::log(Level level, const std::string &message)
{
    std::string time_str = get_current_time();

    // 根据级别选择输出流和前缀
    if (level == ERROR)
    {
        std::cerr << time_str << " [ERROR]   " << message << std::endl;
    }
    else
    {
        const char *level_str = (level == WARNING) ? " [WARNING] " : " [INFO]    ";
        std::cout << time_str << level_str << message << std::endl;
    }
}

std::string Logger::get_current_time()
{
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::tm bt{};
#if defined(_MSC_VER) // Windows (MSVC) 安全函数
    localtime_s(&bt, &time_t_now);
#else // Linux/Unix 安全函数
    localtime_r(&time_t_now, &bt);
#endif

    std::ostringstream oss;
    oss << std::put_time(&bt, "%Y-%m-%d %H:%M:%S");
    oss << "." << std::setfill('0') << std::setw(3) << ms.count();
    return oss.str();
}