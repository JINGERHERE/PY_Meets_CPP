#!/usr/bin/env python
# -*- coding: UTF-8 -*-

# ./src/_color_map.py


import numpy as np
from rich.console import Console


# 指定颜色库
class ColorHub:

    @staticmethod
    def red(text: str) -> str:
        return f"\033[31m{text}\033[0m"

    @staticmethod
    def yellow(text: str) -> str:
        return f"\033[33m{text}\033[0m"

    @staticmethod
    def bold(text: str) -> str:
        return f"\033[1m{text}\033[0m"


# 随机颜色库
def random_color():
    colors = [
        # 红色系
        "#ce181e",
        "#e6194b",
        "#e41a1c",
        "#e71d36",
        "#ff1744",
        "#dd2c00",
        "#ef5350",
        "#ff4c4c",
        # 粉色/洋红系
        "#b84592",
        "#ff4f81",
        "#d20962",
        "#f032e6",
        "#fabebe",
        "#f781bf",
        "#ff4081",
        "#c51162",
        "#f8bbd0",
        "#f06292",
        "#e6beff",
        # 紫色系
        "#8e43e7",
        "#7552cc",
        "#911eb4",
        "#984ea3",
        "#d500f9",
        "#651fff",
        "#ce93d8",
        "#ba68c8",
        "#9575cd",
        "#7986cb",
        "#ab47bc",
        "#9c27b0",
        # 蓝色系
        "#00aeff",
        "#3369e7",
        "#007cc0",
        "#0099e5",
        "#4363d8",
        "#377eb8",
        "#2979ff",
        "#0091ea",
        "#42a5f5",
        "#5c6bc0",
        "#03a9f4",
        "#64b5f6",
        "#4fc3f7",
        # 青色/蓝绿色系
        "#1cc7d0",
        "#00bce4",
        "#46f0f0",
        "#008080",
        "#2ec4b6",
        "#00bfa5",
        "#00b8d4",
        "#00e5ff",
        "#1de9b6",
        "#4dd0e1",
        "#4db6ac",
        "#009688",
        "#8ac926",
        "#bcf60c",
        # 绿色系
        "#2dde98",
        "#34bf49",
        "#00c16e",
        "#3cb44b",
        "#4daf4a",
        "#aaffc3",
        "#00e676",
        "#64dd17",
        "#66bb6a",
        "#81c784",
        "#aed581",
        "#dce775",
        "#d4e157",
        "#26a69a",
        "#69f0ae",
        "#b9f6ca",
        "#aeea00",
        # 黄色/金色系
        "#ffc168",
        "#ffc20e",
        "#ffe119",
        "#ffff33",
        "#ffd600",
        "#fff176",
        "#ffbe0b",
        "#fffac8",
        "#ffd54f",
        "#ffca28",
        # 橙色/珊瑚色系
        "#ff6c5f",
        "#f58231",
        "#f47721",
        "#ff7f00",
        "#ff9f1c",
        "#ff6d00",
        "#ffa726",
        "#ff8a65",
        "#ff7043",
        "#ffd8b1",
        "#ffb74d",
        # 棕色系
        "#9a6324",
        "#a65628",
        "#8d6e63",
        # 中性色系
        "#ffffff",
    ]
    idx = np.random.choice(100)
    return colors[idx]


# 警告颜色定义
console = Console()


def rich_showwarning(message, category, filename, lineno, file=None, line=None):
    # message: Warning 实例；category: Warning 类
    console.print(
        f"[bold yellow]⚠️ [/bold yellow] "
        f"[bold yellow]{category.__name__}:[/bold yellow] "
        f"[bold yellow]{message}[/bold yellow] "
        f"[dim]({filename}:{lineno})[/dim]"
    )


"""
# --------------------------------------------------
# ========== < TEST > ==========
# --------------------------------------------------
"""


if __name__ == "__main__":

    import rich

    for i in range(1000):
        color = random_color()
        rich.print(f"[bold {color}] TEST: Color ! [/bold {color}]")

    import warnings

    # 把 warnings 里的默认 showwarning 换掉
    warnings.showwarning = rich_showwarning
    # 测试
    warnings.warn("这是一个测试警告", UserWarning)