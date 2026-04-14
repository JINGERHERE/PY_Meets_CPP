// ./cpp/base_math/ln2.hpp


#pragma once

/**
 * 使用交错调和级数计算 ln(2)
 * 公式: ln(2) = 1 - 1/2 + 1/3 - 1/4 + ...
 * * @param iter 迭代次数。由于该级数收敛很慢，建议给较大的值。
 * @return 计算出的 ln(2) 近似值
 */
inline double ln2(long long iter = 1000)
{
    double result = 0.0;
    double sign = 1.0;

    for (long long i = 1; i <= iter; ++i)
    {
        // 直接使用 double 进行运算
        result += sign / static_cast<double>(i);

        // 符号翻转
        sign = -sign;
    }

    return result;
}