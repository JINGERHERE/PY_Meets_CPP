#!/usr/bin/env python
# -*- coding: UTF-8 -*-

# ./main.py

import time
import rich
import DemoUtils
import src

color = src.random_color()

def main():
    rich.print(f"[blink]Hello from 'PY_Meets_CPP'![/blink]")


if __name__ == "__main__":

    rich.print(f"\n[bold {color}]## Python Demo TEST: >>> >>> >>> >>> [/bold {color}]")
    main()


    rich.print(f"\n[bold {color}]## TEST: PI >>> >>> >>> >>>[/bold {color}]")
    iteration = 1e5
    # C++ 测量时间
    cpp_start_time = time.time()
    PI = DemoUtils.leibniz_pi(iteration)
    rich.print(f"C++ Leibniz PI: {PI: .6f} in {time.time() - cpp_start_time: .4f} seconds")
    # Python 测量时间
    py_start_time = time.time()
    PI = src.leibniz_pi(int(iteration))
    rich.print(f"Python Leibniz PI: {PI: .6f} in {time.time() - py_start_time: .4f} seconds")



    rich.print(f"\n[bold {color}]## TEST: IntervalCounter >>> >>> >>> >>>[/bold {color}]")
    iter_count = DemoUtils.IntervalCounter(0)

    rich.print(f"Start: {[next(iter_count) for _ in range(5)]}")

    exist_node = [i for i in range(1, int(1e7))]
    rich.print(f"Add history: {exist_node[0]} - {exist_node[-1]}")
    iter_count.add_history(exist_node)

    rich.print(f"Continue: {[next(iter_count) for _ in range(8)]}")
    rich.print(f"Interval History: {iter_count.get_interval_history()}")


    rich.print(f"\n[bold {color}]## TEST: Logger >>> >>> >>> >>>[/bold {color}]")
    DemoUtils.Logger.info("Test Logger INFO")
    DemoUtils.Logger.warn("Test Logger WARNING")
    DemoUtils.Logger.error("Test Logger ERROR")

