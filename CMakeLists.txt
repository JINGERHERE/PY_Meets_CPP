# ./CMakeLists.txt

# 基础环境声明
cmake_minimum_required(VERSION 3.15) # 最低 CMake 版本要求

# 指定编译器为 clang，需要在 project() 之前
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

# 指定编译器为 GCC
# set(CMAKE_C_COMPILER gcc)
# set(CMAKE_CXX_COMPILER g++)

# 项目基本信息
project(PY_MEETS_CPP VERSION 1.0 LANGUAGES CXX) # LANGUAGES CXX 表示项目使用 C++ 语言
set(CMAKE_CXX_STANDARD 23) # 开启 C++23 标准支持
set(CMAKE_CXX_STANDARD_REQUIRED ON) # 强制要求使用 C++23 标准

# 配置 pybind11 找到 Python 解释器
set(PYBIND11_FINDPYTHON ON)

# 查找 pybind11 库
find_package(pybind11 CONFIG REQUIRED) # 去系统或 vcpkg 目录里寻找 pybind11

# 包含 cpp 目录
add_subdirectory(cpp)