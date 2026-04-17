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

# ─────────────────────────────────────────────
# 库类型的平台选择
# ─────────────────────────────────────────────
# Windows 上动态库（.dll）有一个特殊要求：
# 导出给外部使用的函数/类必须用 __declspec(dllexport) 标记（编译 dll 时）
# 使用方必须用 __declspec(dllimport) 标记（编译使用方时）
# 如果头文件里没有这些标记，MSVC 链接时会报"无法解析的外部符号"
#
# 所有平台统一使用 SHARED 动态库
# Windows 上不再退回 STATIC，通过头文件里的 UTILS_API 宏处理导出问题
add_library(utils_lib SHARED interval_counter.cpp)

# UTILS_LIB_EXPORTS：只在编译 utils_lib.dll 本身时定义
# cmake 把它传给编译器（等价于 -DUTILS_LIB_EXPORTS）
# 头文件里的 #ifdef UTILS_LIB_EXPORTS 就是检测这个宏
# 使用方编译时不定义这个宏，头文件里自动切换到 dllimport
target_compile_definitions(utils_lib PRIVATE UTILS_LIB_EXPORTS)

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

# Mac/Linux 动态库：控制符号可见性（可选，不影响功能）
# 打开后只有显式标记的符号对外可见，减少符号污染
# if(NOT WIN32)
#     set_target_properties(utils_lib PROPERTIES CXX_VISIBILITY_PRESET hidden)
# endif()