// ./cpp/base_math/taylor_base.hpp


#pragma once
#include <cmath>

/**
 * 使用泰勒级数计算自然对数底数 e
 * @param iter 迭代次数。对于 double 类型，15-20 次迭代即可达到最大精度。
 * @return 计算出的 e 值
 */

inline double taylor_exp(double iter = 20.0)
{
    long long iterations = static_cast<long long>(iter);

    // 处理特殊情况
    if (iterations < 0)
        return 0.0;
    if (iterations == 0)
        return 1.0;

    double e_estimate = 1.0; // 对应级数的第一项 1/0!
    double factorial = 1.0;  // 用于累积计算阶乘的分母

    for (long long i = 1; i < iterations; ++i)
    {
        factorial *= i; // 更新阶乘：1!, 2!, 3!...
        e_estimate += 1.0 / factorial;
    }

    return e_estimate;
}

/**
 * 使用泰勒级数计算 e^x
 * @param x 指数
 * @param iter 迭代次数（项数）
 */
inline double taylor_expx(double x, int iter = 20)
{
    double result = 1.0; // 第一项 (x^0 / 0!)
    double term = 1.0;   // 记录当前项的值

    for (int i = 1; i < iter; ++i)
    {
        // 关键优化：第 n 项 = 第 n-1 项 * (x / i)
        // 这样避免了重复计算复杂的阶乘和幂运算
        term *= (x / i);
        result += term;
    }
    return result;
}

/**
 * 泰勒级数计算 sin(x)
 */
inline double taylor_sin(double x, int iter = 10)
{
    double result = x;
    double term = x;
    double x_squared = x * x;

    for (int i = 1; i < iter; ++i)
    {
        // sin 展开式：x - x^3/3! + x^5/5! ...
        // 每一项比前一项多了：-(x^2) / (2i * (2i + 1))
        term *= -x_squared / ((2.0 * i) * (2.0 * i + 1.0));
        result += term;
    }
    return result;
}