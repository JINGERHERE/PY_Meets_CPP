// ./cpp/bindings.cpp


// pybind11 头文件
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

// 基础数学 - Header-Only
#include "base_math/ln2.hpp"
#include "base_math/leibniz_pi.hpp"
#include "base_math/taylor_base.hpp"

// 静态类 - 静态库
#include "static_class/logger.h"

// 工具集 - 动态库
#include "utils/interval_counter.h"

// DataFrame 是 header-only，直接 include 头文件就能用
// #include <DataFrame/DataFrame.h>
// #include <DataFrame/DataFrameStatsVisitors.h>


namespace py = pybind11;

PYBIND11_MODULE(DemoUtils, m)
{
    m.doc() = "DemoUtils : 工具集示例";

    // ## 基础数学 ##
    m.def(
        "ln2", &ln2,
        py::arg("iter") = 1000,
        "使用交错调和级数计算 ln(2)"
    );
    
    m.def(
        "leibniz_pi", &leibniz_pi,
        py::arg("iter") = 1000,
        "使用莱布尼兹级数计算 π"
    );

    m.def(
        "taylor_exp", &taylor_exp,
        py::arg("iter") = 20.0,
        "使用泰勒级数计算自然对数底数 e"
    );

    m.def(
        "taylor_expx", &taylor_expx,
        py::arg("x"),
        py::arg("iter") = 20,
        "使用泰勒级数计算 e^x"
    );

    m.def(
        "taylor_sin", &taylor_sin,
        py::arg("x"),
        py::arg("iter") = 10,
        "使用泰勒级数计算 sin(x)"
    );

    
    // ## 静态类 Logger ##
    // 1. 先定义类对象
    py::class_<Logger> logger_cls(m, "Logger");

    // 2. 关键：注册枚举类型 Level (必须在定义方法之前注册)
    // 绑定到 logger_cls 下，Python 中访问方式为 Logger.Level.INFO
    py::enum_<Logger::Level>(logger_cls, "Level")
        .value("INFO", Logger::INFO)
        .value("WARNING", Logger::WARNING)
        .value("ERROR", Logger::ERROR)
        .export_values(); // 允许 Logger.INFO 直接访问 (提升至 Logger 类作用域)

    // 3. 绑定静态方法
    logger_cls.def_static("info", &Logger::info, py::arg("msg"), "打印 INFO 级别日志")
        .def_static("warn", &Logger::warn, py::arg("msg"), "打印 WARNING 级别日志")
        .def_static("error", &Logger::error, py::arg("msg"), "打印 ERROR 级别日志");


    // ## 工具集 ##
    py::class_<IntervalCounter>(m, "IntervalCounter")

        .def(
            py::init<long long, long long>(),
            py::arg("start") = 0, py::arg("step") = 1
        )

        .def("next", &IntervalCounter::next, "生成下一个可用的计数值")
        .def("add_history", &IntervalCounter::add_history, "手动添加黑名单/占用值")
        .def("get_interval_history", &IntervalCounter::get_interval_history, "获取生成过的历史记录（区间树）")
        .def("get_full_history", &IntervalCounter::get_full_history, "获取生成过的历史记录（完整）")

        // 为了支持 Python 的 next() 内置函数，可以绑定 __next__
        .def("__next__", &IntervalCounter::next)
        .def("__iter__", [](py::object s) { return s; });

}