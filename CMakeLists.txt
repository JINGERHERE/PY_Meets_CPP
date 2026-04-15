# ./CMakeLists.txt

cmake_minimum_required(VERSION 3.15)

# ─────────────────────────────────────────────
# 编译器选择（平台自适应）
# ─────────────────────────────────────────────
# CMAKE_SYSTEM_NAME 由 cmake 自动检测，常见值：
# Darwin  → macOS
# Linux   → Linux
# Windows → Windows
#
# 注意：set(CMAKE_C_COMPILER ...) 必须在 project() 之前才能生效
# project() 调用之后 cmake 已经完成编译器检测，再改无效
#
# Windows 上不手动指定编译器，让 cmake 自动找 MSVC
# 因为 vcpkg 的 Windows 默认 triplet (x64-windows) 就是针对 MSVC 编译的
# 如果强制用 clang 或 mingw gcc，和 vcpkg 库的 ABI 会不兼容，链接会报错
if(NOT WIN32)
    # macOS 和 Linux 使用 clang
    set(CMAKE_C_COMPILER clang)
    set(CMAKE_CXX_COMPILER clang++)

    # 如果想改用 gcc，注释上面两行，取消下面两行的注释
    # set(CMAKE_C_COMPILER gcc)
    # set(CMAKE_CXX_COMPILER g++)
endif()

# Windows 上什么都不设置，cmake 自动找 MSVC（通过 VS2019 的环境变量）
project(PY_MEETS_CPP VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ─────────────────────────────────────────────
# Windows 平台特有配置
# ─────────────────────────────────────────────
if(WIN32)
    # MSVC 编译器的字符集和异常处理配置
    # /utf-8     → 源文件和输出都用 UTF-8，避免中文乱码
    # /EHsc      → 启用标准 C++ 异常处理（pybind11 必须）
    # /MP        → 多核并行编译，加速构建
    add_compile_options(/utf-8 /EHsc /MP)

    # Windows 上动态库导出符号需要显式声明
    # 这个宏告诉 pybind11 在 Windows 下正确处理符号导出
    add_compile_definitions(
        NOMINMAX # 禁用 Windows.h 里的 min/max 宏，避免和 std::min/max 冲突
        WIN32_LEAN_AND_MEAN # 减少 Windows.h 包含的内容，加快编译
    )
endif()

# ─────────────────────────────────────────────
# pybind11 配置
# ─────────────────────────────────────────────
set(PYBIND11_FINDPYTHON ON)
find_package(pybind11 CONFIG REQUIRED)

add_subdirectory(cpp)