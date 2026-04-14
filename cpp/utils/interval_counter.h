// ./cpp/utils/interval_counter.h


#pragma once
#include <vector>
#include <set>
#include <utility> // 用于 std::pair

class IntervalCounter
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