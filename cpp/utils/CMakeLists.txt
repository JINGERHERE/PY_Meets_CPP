# ./cpp/utils/CMakeLists.txt

# add_library 用于定义一个库目标
# 语法：add_library(<目标名> <库类型> <源文件...>)
#
# 库类型选项：
# STATIC  → 静态库 (.a / .lib)
# 编译时把代码直接打包进最终产物，运行时不需要额外文件
# 优点：部署简单；缺点：最终 .so 体积更大，多个模块无法共享同一份代码
#
# SHARED  → 动态库 (.so / .dylib / .dll)
# 编译时只记录符号引用，运行时由系统动态加载
# 优点：多个模块可共享同一份库代码，节省内存；缺点：部署时需保证库文件可被找到
#
# MODULE  → 插件模块，不能被其他目标链接，只能运行时动态加载（pybind11 模块就是这种）
#
# INTERFACE → 纯头文件库，不生成任何二进制，只传递编译信息（头文件路径、宏等）
#
# OBJECT  → 只编译成 .o 目标文件，不打包，供其他目标直接引用

# 改为 SHARED，生成动态库 libutils_lib.dylib（macOS）/ libutils_lib.so（Linux）
add_library(utils_lib SHARED
    interval_counter.cpp
)

# target_include_directories 设置头文件搜索路径
# 语法：target_include_directories(<目标名> <传播性> <路径...>)
#
# 传播性选项：
# PRIVATE   → 路径只对 utils_lib 自身编译可见
# 链接了 utils_lib 的目标不会继承这个路径
# 适合：utils_lib 内部实现文件用到的路径，对外不暴露
#
# PUBLIC    → 路径对 utils_lib 自身以及所有链接它的目标都可见
# 链接了 utils_lib 的目标自动继承这个路径，无需重复声明
# 适合：utils_lib 的头文件里有对外的类/函数声明，使用者需要 include 这些头文件
#
# INTERFACE → 路径只传播给链接 utils_lib 的目标，对 utils_lib 自身不生效
# 适合：纯头文件库（INTERFACE 库），自己不编译，只把路径告诉别人

# 使用 PUBLIC，让链接 utils_lib 的目标（比如 DemoUtils 模块）自动继承这个头文件路径
# 这样 cpp/CMakeLists.txt 里的 DemoUtils 就不需要再单独声明 utils 的 include 路径
target_include_directories(utils_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

# 动态库特有的配置：控制符号可见性
# 默认情况下 macOS/Linux 上动态库的所有符号都是可见的
# 如果想精确控制哪些符号对外暴露（类似 Windows 的 __declspec(dllexport)），
# 可以打开下面这行，然后在头文件里用 __attribute__((visibility("default"))) 标记要导出的符号
# set_target_properties(utils_lib PROPERTIES CXX_VISIBILITY_PRESET hidden)