// ./cpp/utils/interval_counter.cpp


#include "interval_counter.h"
#include <algorithm>

IntervalCounter::IntervalCounter(long long start, long long step)
    : current(start), step(step) {}

long long IntervalCounter::next()
{
    jump_if_needed();
    long long result = current;

    // 记录当前值到区间树
    add_single_value(result);

    // 步进并再次检查是否需要跳过新位置的障碍
    current += step;
    jump_if_needed();

    return result;
}

void IntervalCounter::add_history(const std::vector<long long> &values)
{
    for (auto v : values)
    {
        add_single_value(v);
    }
    jump_if_needed();
}

std::vector<std::pair<long long, long long>> IntervalCounter::get_interval_history() const
{
    std::vector<std::pair<long long, long long>> res;
    for (const auto &i : intervals)
    {
        res.push_back({i.start, i.end});
    }
    return res;
}

std::vector<long long> IntervalCounter::get_full_history() const
{
    std::vector<long long> full_history;
    for (const auto &i : intervals)
    {
        for (long long v = i.start; v <= i.end; v += step)
        {
            full_history.push_back(v);
        }
    }
    return full_history;
}

void IntervalCounter::jump_if_needed()
{
    while (true)
    {
        // 找到第一个起点大于 current 的区间
        auto it = intervals.upper_bound({current, current});

        // 检查前一个区间是否包含 current
        if (it != intervals.begin())
        {
            auto prev = std::prev(it);
            if (current >= prev->start && current <= prev->end)
            {
                // 发现冲突，跳到该区间结束后的下一个位置
                current = prev->end + step;
                continue; // 再次循环，确保跳到的新位置也不在其他区间里
            }
        }
        break;
    }
}

void IntervalCounter::add_single_value(long long v)
{
    long long new_start = v;
    long long new_end = v;

    // 找到第一个不小于当前值的区间位置
    auto it = intervals.lower_bound({new_start, new_end});

    // 1. 向后合并：检查 v 是否与后面的区间相邻或重叠
    while (it != intervals.end() && it->start <= new_end + step)
    {
        new_end = std::max(new_end, it->end);
        it = intervals.erase(it);
    }

    // 2. 向前合并：检查 v 是否与前面的区间相邻或重叠
    if (it != intervals.begin())
    {
        auto prev = std::prev(it);
        if (prev->end + step >= new_start)
        {
            new_start = std::min(new_start, prev->start);
            new_end = std::max(new_end, prev->end);
            intervals.erase(prev);
        }
    }

    // 3. 将合并后的新大区间插入
    intervals.insert({new_start, new_end});
}