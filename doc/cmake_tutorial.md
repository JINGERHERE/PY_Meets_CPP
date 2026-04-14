# CMakeLists 完整教程

本教程以 `PY_Meets_CPP` 项目为例，系统讲解 CMake 构建 pybind11 Python 扩展模块的完整知识体系。所有讲解均基于实际项目代码，可直接照搬到你的项目中。

---

## 目录

1. [CMake 变量体系](#1-cmake-变量体系)
2. [库的类型详解](#2-库的类型详解)
3. [target_* 命令与传播性](#3-target_-命令与传播性)
4. [pybind11 模块配置](#4-pybind11-模块配置)
5. [动态库与 RPATH](#5-动态库与-rpath)
6. [install 命令](#6-install-命令)
7. [完整项目配置流程](#7-完整项目配置流程)
8. [常见问题与调试](#8-常见问题与调试)

---

## 1. CMake 变量体系

### 1.1 常用变量一览

| 变量 | 含义 | 示例 |
|------|------|------|
| `CMAKE_SOURCE_DIR` | 项目根目录（最顶层 `CMakeLists.txt` 所在目录） | `/path/to/PY_Meets_CPP` |
| `CMAKE_CURRENT_SOURCE_DIR` | 当前正在处理的 CMakeLists.txt 所在目录 | `.../cpp/base_math` |
| `PROJECT_SOURCE_DIR` | 最近一次 `project()` 调用所在的目录 | 通常等于 `CMAKE_SOURCE_DIR` |
| `CMAKE_BINARY_DIR` | 构建输出目录（顶级 `CMakeLists.txt` 的构建根目录） | `/path/to/PY_Meets_CPP/build` |
| `CMAKE_CURRENT_BINARY_DIR` | 当前目标的构建输出目录 | `.../cpp/base_math/build` |
| `VCPKG_TARGET_TRIPLET` | vcpkg 目标平台标识 | `x64-linux`, `x64-osx` |

```cmake
# 项目根目录 ./CMakeLists.txt
message(STATUS "项目根目录: ${CMAKE_SOURCE_DIR}")
message(STATUS "当前目录: ${CMAKE_CURRENT_SOURCE_DIR}")

# 在子目录 cpp/CMakeLists.txt 中
message(STATUS "cpp 目录: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "仍指向项目根: ${CMAKE_SOURCE_DIR}")
```

### 1.2 message() 命令：调试与输出

`message()` 用于在 CMake 配置阶段向用户显示信息，类似于 Python 的 `print()` 或 C++ 的 `std::cout`。

**语法**：
```cmake
message(<级别> "消息内容")
```

**消息级别**：

| 级别 | 效果 |
|------|------|
| `STATUS` | 普通信息，带前后缀 `--` |
| `WARNING` | 警告消息，黄色显示，继续执行 |
| `AUTHOR_WARNING` | 开发者警告（需额外 flags） |
| `FATAL_ERROR` | 致命错误，停止配置 |
| `SEND_ERROR` | 错误消息，继续执行但不生成 |
| `DEPRECATION` | 废弃警告 |

**示例**：
```cmake
message(STATUS "开始配置项目")          # -- 开始配置项目
message(WARNING "找不到 Python，使用默认") # CMake Warning: ...
message(FATAL_ERROR "CMake 版本过低")     # CMake Fatal Error: ...
```

**实际项目用法**：
```cmake
# 调试变量值
message(STATUS "VCPKG_TARGET_TRIPLET = ${VCPKG_TARGET_TRIPLET}")

# 打印配置信息
message(STATUS "C++ 标准: ${CMAKE_CXX_STANDARD}")
message(STATUS "构建类型: ${CMAKE_BUILD_TYPE}")
```

### 1.3 编译器指定

**必须在 `project()` 之前指定编译器**，否则 CMake 会使用默认编译器：

```cmake
# 指定 Clang（推荐）
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

# 切换为 GCC
# set(CMAKE_C_COMPILER gcc)
# set(CMAKE_CXX_COMPILER g++)

project(PY_MEETS_CPP VERSION 1.0 LANGUAGES CXX)
```

### 1.4 C++ 标准设置

```cmake
set(CMAKE_CXX_STANDARD 23)           # 开启 C++23
set(CMAKE_CXX_STANDARD_REQUIRED ON)   # 强制要求，不允许降级
```

---

## 2. 库的类型详解

CMake 中通过 `add_library()` 创建库目标，支持五种类型：

### 2.1 INTERFACE（接口库/纯头文件库）

**原理**：不生成任何二进制文件，仅传递编译信息（头文件路径、宏定义）。

**适用场景**：Header-Only 库 —— 只有 `.hpp`/`.h` 文件，无 `.cpp` 源文件。

```cmake
# ./cpp/base_math/CMakeLists.txt
add_library(base_math_lib INTERFACE)  # 空壳库

# 传递头文件搜索路径
target_include_directories(base_math_lib INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
```

```cmake
# 链接方式（使用时）
target_link_libraries(my_module PRIVATE base_math_lib)
# 链接后，my_module 自动继承 base_math_lib 的头文件路径
```

### 2.2 STATIC（静态库）

**原理**：编译时将库代码**拷贝**到最终可执行文件/模块中。链接后库代码成为最终产物的一部分。

**文件后缀**：
- Linux/macOS: `.a`
- Windows: `.lib`

**优点**：部署简单，最终产物独立运行
**缺点**：多个模块无法共享同一份代码，最终产物体积大

```cmake
# ./cpp/static_class/CMakeLists.txt
add_library(static_class_lib STATIC
    logger.cpp
)

target_include_directories(static_class_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
```

### 2.3 SHARED（动态库）

**原理**：编译时只记录符号引用，运行时由系统动态加载。多个模块可共享同一份库代码。

**文件后缀**：
- Linux: `.so`
- macOS: `.dylib`
- Windows: `.dll`

**优点**：节省内存，多模块共享
**缺点**：部署时需保证库文件可被找到

```cmake
# ./cpp/utils/CMakeLists.txt
add_library(utils_lib SHARED
    interval_counter.cpp
)

target_include_directories(utils_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
```

### 2.4 MODULE（插件模块）

**原理**：特殊的动态库，不能被其他 C++ 程序直接链接，只能在运行时通过专用 API 动态加载（如 Python 的 `import`、Linux 的 `dlopen`）。

**pybind11 默认使用 MODULE 类型**：`pybind11_add_module` 内部就是创建 MODULE 类型的库。

```cmake
# Python 扩展模块就是 MODULE
pybind11_add_module(DemoUtils bindings.cpp)  # 等价于 add_library(DemoUtils MODULE bindings.cpp)
```

### 2.5 OBJECT（对象库）

**原理**：只编译成 `.o` 目标文件，不打包。供其他目标直接引用编译结果。

```cmake
add_library(my_objects OBJECT a.cpp b.cpp)  # 生成 .o 文件
add_library(my_lib STATIC $<TARGET_OBJECTS:my_objects>)  # 链接到静态库
```

### 2.6 类型对比总结

| 类型 | 二进制文件 | 链接方式 | 典型用途 |
|------|-----------|----------|----------|
| INTERFACE | 无 | 只传播信息 | Header-Only 库 |
| STATIC | `.a` / `.lib` | 拷贝进最终产物 | 不希望暴露源码的工具类 |
| SHARED | `.so` / `.dylib` / `.dll` | 运行时加载，多模块共享 | 需要被多个模块使用的库 |
| MODULE | `.so` / `.dylib` | 运行时专用 API 加载 | Python 扩展、插件系统 |
| OBJECT | `.o` | 不打包，直接引用 | 中间编译产物复用 |

---

## 3. target_* 命令与传播性

### 3.1 target_include_directories

**语法**：
```cmake
target_include_directories(<目标> <传播性> <路径...>)
```

**传播性选项**：

| 选项 | 含义 | 适用场景 |
|------|------|----------|
| `PRIVATE` | 路径仅对当前目标可见，链接当前目标的其他目标不会继承 | 内部实现用到的路径，不对外暴露 |
| `PUBLIC` | 路径对当前目标及所有链接它的目标都可见 | 库的头文件路径，链接者需要知道 |
| `INTERFACE` | 路径只传播给链接者，对自身不生效 | 纯头文件库（INTERFACE 库专用） |

**示例解析**：

```cmake
# utils 库使用 PUBLIC——链接它的目标自动继承路径
target_include_directories(utils_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

# cpp/CMakeLists.txt 中链接时：
target_link_libraries(DemoUtils PRIVATE utils_lib)
# DemoUtils 自动继承 utils_lib 的 PUBLIC 路径，无需再写一遍 include_directories
```

### 3.2 target_link_libraries

**语法**：
```cmake
target_link_libraries(<目标> <传播性> <库...>)
```

**传播性对链接同样适用**：
- `PRIVATE`：只给当前目标链接，继承者不继承
- `PUBLIC`：当前目标和继承者都链接
- `INTERFACE`：只传播给继承者，自己不链接

**实际项目示例**：

```cmake
# ./cpp/CMakeLists.txt
# base_math_lib  是 INTERFACE 库（纯头文件），链接它只是为了继承头文件路径
# static_class_lib 是 STATIC 库，代码会被直接打包进 DemoUtils.so
# utils_lib      是 SHARED 库，运行时动态加载，DemoUtils.so 只记录对它的引用
target_link_libraries(${target_name} PRIVATE base_math_lib static_class_lib utils_lib)
```

### 3.3 target_compile_definitions

传递宏定义：
```cmake
target_compile_definitions(my_lib PUBLIC ENABLE_LOGGING)
```

### 3.4 target_compile_options

传递编译器选项：
```cmake
target_compile_options(my_lib PRIVATE -Wall -Wextra)
```

---

## 4. pybind11 模块配置

### 4.1 查找 pybind11

**方式一：通过 vcpkg（推荐）**
```cmake
set(PYBIND11_FINDPYTHON ON)
find_package(pybind11 CONFIG REQUIRED)
```

**方式二：通过 CMake ExternalProject**
```cmake
include(FetchContent)
FetchContent_Declare(pybind11 GIT_REPOSITORY https://github.com/pybind/pybind11 ...)
FetchContent_MakeAvailable(pybind11)
```

### 4.2 创建 Python 扩展模块

```cmake
# pybind11_add_module 是 pybind11 提供的便捷命令
# 等价于 add_library(DemoUtils MODULE bindings.cpp) + 一堆 pybind11 专属配置
pybind11_add_module(${target_name} bindings.cpp)
```

### 4.3 头文件路径配置

```cmake
target_include_directories(${target_name} PRIVATE
    # vcpkg 安装后的标准头文件路径
    "${CMAKE_SOURCE_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/include"

    # 也可以使用系统路径（如 /usr/local/include）
)
```

### 4.4 pybind11 bindings.cpp 编写要点

```cpp
// 固定写法：创建名为 DemoUtils 的 Python 模块
PYBIND11_MODULE(DemoUtils, m)
{
    m.doc() = "模块文档字符串";

    // 暴露普通函数
    m.def("function_name", &cpp_function, py::arg("param") = default_value, "文档");

    // 暴露类
    py::class_<MyClass>(m, "MyClass")
        .def(py::init<>())  // 构造函数
        .def("method", &MyClass::method);

    // 绑定枚举（必须先注册）
    py::class_<MyClass> cls(m, "MyClass");
    py::enum_<MyEnum>(cls, "MyEnum")
        .value("A", MyEnum::A)
        .export_values();  // 允许 MyClass.MyEnum.A 直接访问
}
```

---

## 5. 动态库与 RPATH

### 5.1 为什么需要 RPATH

当 `DemoUtils.so`（pybind11 模块）依赖 `libutils_lib.dylib`（动态库）时：

- **问题**：运行时系统只在 `/usr/lib` 等标准路径查找，找不到则报错
- **解决**：在 `DemoUtils.so` 中嵌入 RPATH，告知系统去哪个目录找依赖库

### 5.2 @loader_path 与 $ORIGIN

| 平台 | 变量 | 含义 |
|------|------|------|
| macOS | `@loader_path` | "当前 .so 文件所在的目录" |
| Linux | `$ORIGIN` | "当前 .so 文件所在的目录" |

```cmake
# ./cpp/CMakeLists.txt
set_target_properties(${target_name} PROPERTIES
    # macOS 用 @loader_path
    INSTALL_RPATH "@loader_path"

    # 构建阶段也生效（不设置只在 install 后才生效）
    BUILD_WITH_INSTALL_RPATH TRUE
)
```

**效果**：
```
# 最终目录结构
site-packages/DemoUtils.cpython-38-x86_64-linux-gnu.so  # 主模块
site-packages/libutils_lib.so                           # 动态库（在同一目录）
```

运行时 `@loader_path` 指向 `site-packages/`，系统能找到 `libutils_lib.so`。

### 5.3 CMAKE_INSTALL_RPATH 与 BUILD_WITH_INSTALL_RPATH

| 属性 | 作用时机 | 用途 |
|------|----------|------|
| `INSTALL_RPATH` | `install` 之后生效 | 指定安装后的 RPATH |
| `BUILD_WITH_INSTALL_RPATH TRUE` | 构建阶段也生效 | 让 `cmake --build .` 直接运行时也能找到库 |

```cmake
# 常见配置组合
set_target_properties(target PROPERTIES
    INSTALL_RPATH "@loader_path"  # macOS
    BUILD_WITH_INSTALL_RPATH TRUE  # 开发时直接运行也能找到库
)
```

### 5.4 符号可见性（可选）

macOS/Linux 动态库默认所有符号可见。如需精确控制导出哪些符号：

```cmake
# CMakeLists.txt
set_target_properties(utils_lib PROPERTIES CXX_VISIBILITY_PRESET hidden)

# 头文件中标记要导出的符号
__attribute__((visibility("default"))) void my_exported_function();
```

---

## 6. install 命令

### 6.1 基本语法

```cmake
install(TARGETS <目标> DESTINATION <路径>)
```

**DESTINATION 的含义**：
- 相对路径：相对于 `CMAKE_INSTALL_PREFIX`（Python 包根目录）
- `.`（点号）：安装到 `CMAKE_INSTALL_PREFIX` 的根目录

**scikit-build-core 约定**：`DESTINATION .` 表示安装到 Python 包的根目录。

### 6.2 安装主模块

```cmake
# ./cpp/CMakeLists.txt
# 安装 DemoUtils.so 到 Python 包根目录
install(TARGETS ${target_name} DESTINATION .)
```

### 6.3 同时安装动态库依赖

```cmake
# 动态库也需要一起安装，配合 RPATH 才能被正确加载
install(TARGETS utils_lib DESTINATION .)
```

### 6.4 安装头文件（可选）

```cmake
# 安装头文件到 include 目录
install(FILES my_header.h DESTINATION include/myproject)
```

---

## 7. 完整项目配置流程

### 7.1 项目层级结构

```
PY_Meets_CPP/
├── CMakeLists.txt                  # 顶层配置
└── cpp/
    ├── CMakeLists.txt              # 主模块配置
    ├── base_math/                  # INTERFACE 库
    │   └── CMakeLists.txt
    ├── static_class/              # STATIC 库
    │   └── CMakeLists.txt
    └── utils/                      # SHARED 库
        └── CMakeLists.txt
```

### 7.2 步骤一：顶层 CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.15)

# 编译器配置（必须放在 project() 之前）
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

project(PY_MEETS_CPP VERSION 1.0 LANGUAGES CXX)

# C++ 标准
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# pybind11 配置
set(PYBIND11_FINDPYTHON ON)
find_package(pybind11 CONFIG REQUIRED)

# 包含 cpp 子目录
add_subdirectory(cpp)
```

### 7.3 步骤二：子模块 CMakeLists.txt

**Header-Only（INTERFACE）**：
```cmake
# ./cpp/base_math/CMakeLists.txt
add_library(base_math_lib INTERFACE)

# INTERFACE 库使用 INTERFACE 传播性
target_include_directories(base_math_lib INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
```

**STATIC 库**：
```cmake
# ./cpp/static_class/CMakeLists.txt
add_library(static_class_lib STATIC
    logger.cpp
)

target_include_directories(static_class_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
```

**SHARED 库**：
```cmake
# ./cpp/utils/CMakeLists.txt
add_library(utils_lib SHARED
    interval_counter.cpp
)

# PUBLIC 让链接 utils_lib 的目标自动继承这个路径
target_include_directories(utils_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
```

### 7.4 步骤三：主模块 CMakeLists.txt

```cmake
# ./cpp/CMakeLists.txt
add_subdirectory(base_math)
add_subdirectory(static_class)
add_subdirectory(utils)

# 创建 pybind11 模块
set(target_name DemoUtils)
pybind11_add_module(${target_name} bindings.cpp)

# 链接所有库
target_link_libraries(${target_name} PRIVATE
    base_math_lib      # INTERFACE 库：传递头文件路径
    static_class_lib    # STATIC 库：代码打包进模块
    utils_lib          # SHARED 库：运行时动态加载
)

# 头文件路径
target_include_directories(${target_name} PRIVATE
    "${CMAKE_SOURCE_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/include"
)

# RPATH 配置（macOS）
set_target_properties(${target_name} PROPERTIES
    INSTALL_RPATH "@loader_path"
    BUILD_WITH_INSTALL_RPATH TRUE
)

# 安装主模块
install(TARGETS ${target_name} DESTINATION .)

# 安装动态库
install(TARGETS utils_lib DESTINATION .)
```

---

## 8. 常见问题与调试

### 8.1 CMake 找不到 pybind11

**原因**：vcpkg 未正确安装或未集成。

**解决**：
```bash
# 1. 确保 vcpkg 安装完成
./vcpkg_install.command

# 2. 集成到系统（首次）
./vcpkg integrate install

# 3. 清理缓存重新构建
rm -rf build/
uv sync --all-groups
```

### 8.2 运行时 `ImportError: libutils_lib.dylib not found`

**原因**：动态库未安装到正确位置，或 RPATH 配置有误。

**排查步骤**：
1. 检查 `libutils_lib.dylib` 是否与 `DemoUtils.so` 在同一目录
2. 确认 `INSTALL_RPATH` 设置为 `@loader_path`（macOS）
3. 确认 `BUILD_WITH_INSTALL_RPATH TRUE` 已设置

**调试命令**：
```bash
# 查看库的依赖关系（macOS）
otool -L DemoUtils.so

# 查看 RPATH（macOS）
otool -l DemoUtils.so | grep -A 2 RPATH

# 查看库的依赖关系（Linux）
ldd DemoUtils.so
```

### 8.3 编译报错 "undefined symbol"

**原因**：链接了错误的库类型。

**检查**：
- 静态库（STATIC）→ 代码应被拷贝进最终产物
- 动态库（SHARED）→ 运行时需能找到
- pybind11 模块（MODULE）→ 不能被其他 C++ 程序链接

### 8.4 头文件路径传递失效

**检查**：
- INTERFACE 库必须用 `INTERFACE` 传播性
- 链接时应使用 `PRIVATE` 或 `PUBLIC`，不能用 `INTERFACE` 链接 INTERFACE 库

**正确**：
```cmake
target_link_libraries(my_module PRIVATE base_math_lib)  # 继承路径
```

### 8.5 CMake 变量为空

**原因**：变量未定义或拼写错误。

**调试**：
```cmake
message(STATUS "DEBUG: CMAKE_SOURCE_DIR = ${CMAKE_SOURCE_DIR}")
message(STATUS "DEBUG: VCPKG_TARGET_TRIPLET = ${VCPKG_TARGET_TRIPLET}")
```

### 8.6 多平台构建

**Linux**：
```cmake
set_target_properties(${target_name} PROPERTIES
    INSTALL_RPATH "$ORIGIN"
)
```

**Windows**：通常不需要 RPATH，动态库在同一目录即可。

---

## 附录：scikit-build-core 配置

`pyproject.toml` 中的 CMake 相关配置：

```toml
[tool.scikit-build]
cmake.build-type = "Release"           # 构建类型：Debug/Release
cmake.define.CMAKE_EXPORT_COMPILE_COMMANDS = "ON"  # 生成 compile_commands.json
build-dir = "build/{build_type}"       # 构建输出目录
```

**效果**：
- `uv sync` 会自动调用 CMake 构建 C++ 代码
- 产物自动安装到 Python 包目录
- `.so`/`.pyd` 文件可以直接 `import DemoUtils`