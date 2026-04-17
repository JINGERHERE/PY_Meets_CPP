// ./cpp/utils/interval_counter.h

#pragma once
#include <vector>
#include <set>
#include <utility>

// ─────────────────────────────────────────────
// DLL 导出宏
// Windows 上编译动态库时，对外暴露的符号必须显式标记
// 否则 MSVC 链接时会报"无法解析的外部符号"
//
// 工作原理：
//   编译 utils_lib.dll 本身时：cmake 会自动定义 UTILS_LIB_EXPORTS 宏
//     -> __declspec(dllexport) 生效，告诉编译器把这个符号写入导出表
//   其他目标链接 utils_lib.dll 时：UTILS_LIB_EXPORTS 未定义
//     -> __declspec(dllimport) 生效，告诉编译器从外部 dll 导入这个符号
//
// Linux/macOS 上动态库默认所有符号可见，这个宏展开为空，不影响任何东西
// ─────────────────────────────────────────────
#if defined(_WIN32) || defined(_WIN64)
#ifdef UTILS_LIB_EXPORTS
// 正在编译 dll 本身
#define UTILS_API __declspec(dllexport)
#else
// 其他目标在使用这个 dll
#define UTILS_API __declspec(dllimport)
#endif
#else
// Linux / macOS：展开为空，无任何影响
#define UTILS_API
#endif


class UTILS_API IntervalCounter
{
public:
    // 定义区间结构
    struct Interval
    {
        long long start;
        long long end;
        // 按起点排序，用于 set 的自动排序和二分查找
        bool operator<(const Interval &other) const
        {
            return start < other.start;
        }
    };

    /**
     * @param start 计数起始值
     * @param step 步长
     */
    IntervalCounter(long long start = 0, long long step = 1);

    // 生成下一个未被占用的值
    long long next();

    // 批量添加已被占用的历史值
    void add_history(const std::vector<long long> &values);

    // 获取压缩后的区间历史（极其节省内存）
    std::vector<std::pair<long long, long long>> get_interval_history() const;

    // 获取展开后的完整历史（按需生成，时间换空间）
    std::vector<long long> get_full_history() const;

private:
    long long current;
    long long step;
    std::set<Interval> intervals;

    // 内部逻辑：如果当前值被占用，则跳跃到下一个可用位置
    void jump_if_needed();

    // 内部逻辑：将单个值合并到区间集合中
    void add_single_value(long long v);
};