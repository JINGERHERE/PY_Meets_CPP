# 当 Python 遇到 C++

> 学习示例 —— 如何在 Python 中调用 C++ 编写的库

> 已在 `Apple M2 - macOS 15.5` 、 `Windows 11 (x64) - MSVC 19.33` 和 `Ubuntu 24.04 (Container)` 上测试通过

本项目演示了如何通过 pybind11 将 C++ 代码编译为 Python 可调用的扩展模块，并使用 scikit-build-core 作为构建后端，vcpkg 管理 C++ 依赖，uv 管理 Python 环境。

**核心目标**：将项目所需的全部依赖和环境限制在项目根目录下，避免污染全局系统环境。

> 在开发容器中应该可以直接在构建容器时`apt install python`来指定 python 版本，毕竟容器已经和宿主机有隔离，不必再使用 `uv` 隔离一下。

---

## 相关文档

- [CMakeLists 完整教程](doc/cmake_tutorial.md) — 详解本项目所有 CMakeLists.txt 的配置思路与 CMake 知识点

---

## 项目架构

```
PY_Meets_CPP/
├── CMakeLists.txt              # 顶层构建配置
├── pyproject.toml              # Python 项目配置 + scikit-build-core
├── vcpkg.json                  # C++ 依赖声明
├── vcpkg-configuration.json    # vcpkg 源配置
├── uv.lock                     # Python 依赖锁定文件
├── LICENSE                     # MIT 许可证
├── .gitattributes              # Git 属性配置（换行符处理）
│
├── cpp/                        # C++ 源代码目录
│   ├── CMakeLists.txt          # cpp 子目录构建配置
│   ├── bindings.cpp            # pybind11 绑定入口
│   │
│   ├── base_math/              # 基础数学库（Header-Only）
│   │   ├── CMakeLists.txt
│   │   ├── ln2.hpp             # 交错调和级数计算 ln(2)
│   │   ├── leibniz_pi.hpp      # 莱布尼兹级数计算 π
│   │   └── taylor_base.hpp     # 泰勒级数（exp, e^x, sin）
│   │
│   ├── static_class/           # 静态类库
│   │   ├── CMakeLists.txt
│   │   ├── logger.h            # Logger 类声明
│   │   └── logger.cpp         # Logger 类实现
│   │
│   └── utils/                  # 工具库（动态库）
│       ├── CMakeLists.txt
│       ├── interval_counter.h  # IntervalCounter 类声明
│       └── interval_counter.cpp # IntervalCounter 类实现
│
├── src/                        # Python 纯源码模块
│   ├── __init__.py
│   ├── _leibniz_pi.py          # Python 版莱布尼兹级数
│   └── _color_map.py           # 颜色工具
│
├── doc/                        # 文档
│   └── cmake_tutorial.md       # CMakeLists 完整教程
│
├── main.py                     # Python 演示入口
│
├── build/                      # 构建产物目录（自动生成）
│
├── vcpkg_installed/            # vcpkg 已安装库目录（自动生成）
│
├── install_vcpkg.sh            # vcpkg 安装脚本（通用 Shell）
├── install_vcpkg.command       # vcpkg 安装脚本（macOS/Linux）
├── install_vcpkg.bat           # vcpkg 安装脚本（Windows）
├── build.sh                    # 一键构建脚本（通用 Shell）
├── build.command               # 一键构建脚本（macOS/Linux）
└── build.bat                   # 一键构建脚本（Windows）
```

---

## 核心模块说明

### 1. base_math —— Header-Only 库

纯头文件库，不生成二进制文件，仅提供数学计算函数：

| 函数 | 说明 |
|------|------|
| `ln2(iter=1000)` | 交错调和级数计算 ln(2) |
| `leibniz_pi(iter=1000)` | 莱布尼兹级数计算 π |
| `taylor_exp(iter=20)` | 泰勒级数计算自然常数 e |
| `taylor_expx(x, iter=20)` | 泰勒级数计算 e^x |
| `taylor_sin(x, iter=10)` | 泰勒级数计算 sin(x) |

**设计要点**：由于仅含模板和内联函数，无须编译，直接被 `bindings.cpp` include 使用。

### 2. static_class —— 静态库

Logger 静态类，提供带时间戳的日志输出功能：

```python
DemoUtils.Logger.info("消息")    # INFO 级别
DemoUtils.Logger.warn("警告")     # WARNING 级别
DemoUtils.Logger.error("错误")    # ERROR 级别
```

**设计要点**：编译为静态库（`.a`/`.lib`），链接时代码直接拷贝进最终模块，不产生额外依赖。

### 3. utils —— 动态库

`IntervalCounter` 区间计数器类，用于生成连续不冲突的整数序列：

```python
counter = DemoUtils.IntervalCounter(start=0, step=1)
counter.add_history([5, 6, 7])        # 标记这些值已被占用
counter.next()                         # 返回下一个可用值
counter.get_interval_history()        # 获取区间形式的历史记录
counter.get_full_history()            # 获取完整展开的历史记录
```

**设计要点**：编译为动态库（`.so`/`.dylib`），运行时由 Python 模块动态加载，与主模块分离。

### 4. bindings.cpp —— pybind11 绑定层

所有 C++ 函数的 Python 入口，负责：
- 暴露函数和类到 Python
- 处理类型转换（pybind11 自动处理 STL 容器）
- 注册枚举类型、迭代器等

---

## 构建要求

| 工具 | 要求 | 说明 |
|------|------|------|
| **编译器** | GCC 或 Clang | 需支持 C++23 |
| **CMake** | ≥ 3.15 | 构建系统 |
| **vcpkg** | 最新稳定版 | C++ 依赖管理 |
| **uv** | 最新稳定版 | Python 环境管理 |
| **Python** | 3.8 ~ 3.8.x | 项目配置固定版本 |

> **注意**：
> 1. 本项目 Python 版本在 `pyproject.toml` 中锁定为 `>=3.8, <3.9`（即 3.8.x），配置如此。如需更换 Python 版本，请修改`requires-python` 字段。
> 2. 编译 C++ 代码时使用的 pybind11 由 uv 环境提供（便于 Python 版本控制），vcpkg 安装的 pybind11 仅用于提供头文件，不参与编译。

---

## 快速开始

### 方式一：一键构建（推荐）

```bash
# 项目根目录下执行
chmod +x build.command   # 首次运行需要赋予执行权限
./build.command
```

构建脚本会自动：
1. 清理旧构建产物
2. 通过 vcpkg 安装 C++ 依赖（pybind11, dataframe）
3. 通过 uv sync 同步 Python 环境并触发 C++ 构建
4. 生成 `.pyi` 存根文件（供 IDE 自动补全）
5. 运行 `main.py` 验证

### 方式二：分步执行

```bash
# 1. 安装 C++ 依赖
vcpkg install

# 2. 同步 Python 环境
uv sync --all-groups

# 3. 手动运行演示
uv run python main.py
```

---

## 依赖说明

### C++ 依赖（vcpkg）

| 依赖 | 版本 | 用途 |
|------|------|------|
| fmt | latest | 格式化输出库（未使用，仅用于测试依赖安装） |

### Python 依赖（pyproject.toml）

| 依赖 | 版本 | 用途 |
|------|------|------|
| jupyter | ≥ 1.1.1 | 交互式编程环境 |
| marimo | ≥ 0.17.6 | 交互式 Python 笔记本 |
| matplotlib | ≥ 3.7.5 | 绘图 |
| numpy | ≥ 1.24.4 | 数值计算 |
| pandas | ≥ 2.0.3 | 数据处理 |
| rich | ≥ 14.3.4 | 终端美化输出 |

### 构建系统依赖

| 依赖 | 版本 | 用途 |
|------|------|------|
| scikit-build-core | ≥ 0.9.0 | CMake-based Python build backend |
| pybind11 | ≥ 3.0.3 | Python-C++ 绑定（构建时使用） |

### 开发依赖（dependency-groups）

| 依赖 | 用途 |
|------|------|
| pybind11 | pybind11 开发头文件（由 uv 管理） |
| pybind11-stubgen | 生成 `.pyi` 存根文件 |

---

## 库类型区分

本项目展示了三种典型的库类型组合：

| 库 | 类型 | 链接方式 | 适用场景 |
|----|------|----------|----------|
| base_math | INTERFACE | 纯头文件，编译时传递路径 | 模板库、内联函数库 |
| static_class | STATIC | 代码拷贝进最终产物 | 不希望暴露源码的工具类 |
| utils | SHARED | 运行时动态加载 | 需要多模块共享的库 |

**区分说明**

- **Header-Only（INTERFACE）**：避免编译单元重复，模板代码必须头文件可见
- **静态库**：Logger 等简单工具，代码量小，直接打包更简单
- **动态库**：utils 可能被其他模块复用，动态加载节省内存

---

## 故障排查

### 1. `ModuleNotFoundError: DemoUtils`

确保：
- 构建成功完成，无报错
- 当前在项目根目录运行 Python
- `uv run python main.py` 而非直接 `python main.py`

### 2. 运行时找不到动态库（如 `libutils_lib.dylib`）

CMake 已配置 RPATH 为 `@loader_path`（macOS），确保：
- `utils_lib` 和 `DemoUtils` 在同一目录下
- 使用 `uv sync` 构建会正确复制到 Python 包目录

### 3. vcpkg 安装失败

```bash
# 拉取最新 vcpkg
git pull
./vcpkg integrate install  # 首次使用需要集成
```

### 4. CMake 版本过旧

```bash
# 使用 vcpkg 提供的 CMake
export PATH="$PWD/vcpkg/downloads/tools/cmake-*/cmake-*/bin:$PATH"
```