// ./cpp/base_math/leibniz_pi.hpp


#pragma once

/**
 * @brief 使用莱布尼兹级数计算 π
 * @param iter 迭代次数，支持 double 以兼容 Python 的 float 科学计数法
 */

// double leibniz_pi(double iter = 10.0)
// {
//     // 将传入的浮点数安全转为长整型
//     long long iterations = static_cast<long long>(iter);

//     double result = 0.0;
//     for (long long i = 0; i < iterations; i++)
//     {
//         double term = (i % 2 == 0) ? 1.0 : -1.0;
//         result += term / (2.0 * i + 1.0);
//     }
//     result *= 4.0;
//     return result;
// }

inline double leibniz_pi(double iter = 10.0)
{
    long long iterations = static_cast<long long>(iter);
    double result = 0.0;
    double sign = 1.0;

    for (long long i = 0; i < iterations; i++)
    {
        result += sign / (2.0 * i + 1.0);
        sign = -sign;
    }
    return result * 4.0;
}