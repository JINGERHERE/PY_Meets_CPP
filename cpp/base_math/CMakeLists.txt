# ./cpp/base_math/CMakeLists.txt


# 生成 math_lib 库
add_library(base_math_lib INTERFACE) # 当前路径下只有 hpp 文件

#[[
STATIC(静态库)
    后缀：Windows 上是 .lib，Linux/macOS 上是 .a。
    原理：编译器把你的代码打包成一个压缩包。当你的程序（Executable）链接这个静态库时，链接器会把库里用到的二进制代码直接拷贝一份，嵌入到最终的可执行文件中。

SHARED(动态库)
    后缀：Windows 上是 .dll，Linux 上是 .so，macOS 上是 .dylib。
    原理：代码不被拷贝进可执行文件。可执行文件只记录一个“索引”。程序运行时，系统会去指定的文件夹里找这个库并加载。

MODULE (插件模块)
    原理：这是一种特殊的动态库。它不能被其他 C++ 程序直接链接（Link），只能在程序运行过程中被“动态加载”（像 Python 的 import）。
    用途：这正是 pybind11 的核心用法！ 当你使用 pybind11_add_module 时，CMake 默认就是把它配置为 MODULE 类型，这样 Python 才能通过 import 调用它。

INTERFACE (接口库)
    原理：这通常是一个“空壳”或者“纯头文件库”。它不生成任何二进制文件。
    用途：如果你有一个文件夹里全是 .hpp 模板文件，你就定义一个 INTERFACE 库，用来传递路径和宏定义。
]]

# 包含头文件路径
target_include_directories(base_math_lib INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

#[[
target_include_directories 的含义：
    当编译 math_lib 时，会去指定的目录里找头文件。
    如果是 PRIVATE 则表示当 main 链接 math_lib 时，因为是 PRIVATE，CMake 不会把这个路径传给 main。
        结果就是虽然 main 知道 math_lib 的二进制代码在哪（链接阶段没问题），但在编译阶段，main 根本不知道 leibniz.h 这个物理文件在哪里。
]]

#[[
PRIVATE： 
    表示这个目录只对 math_lib 这个目标可见，其他目标不能访问

PUBLIC： 
    表示这个目录对所有目标都是可见的，相当于在外层使用“leibniz.h”时，不需要添加头文件路径。
    如果你的库提供了一些接口类或函数声明，而这些声明都在头文件里，那么链接你这个库的人必然需要包含你的头文件。这时候必须用 PUBLIC。

INTERFACE： 
    通常用于 Header-only 库。这个库本身不需要编译（没有 .cpp），它存在的唯一目的就是把头文件路径传给别人。
]]