#!/usr/bin/env python
# -*- coding: UTF-8 -*-

# ./src/_leibniz_pi.py


def leibniz_pi(iterations: int) -> float:
    """
    计算 π 的近似值，使用莱布尼兹级数。

    :param iterations: 迭代次数，必须为整数。
    :return: π 的近似值。
    """

    result = 0.0

    for i in range(iterations):
        term = 1.0 if i % 2 == 0 else -1.0
        result += term / (2.0 * i + 1.0)
    
    return result * 4.0