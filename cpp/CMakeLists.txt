# ./cpp/CMakeLists.txt

# 包含子目录
add_subdirectory(base_math)
add_subdirectory(static_class)
add_subdirectory(utils) # 动态库

# 目标文件名
set(target_name DemoUtils)

# pybind11_add_module 生成 Python 扩展模块（MODULE 类型的动态库）
# 语法：pybind11_add_module(<目标名> <源文件...>)
# 这个命令等价于 add_library(DemoUtils MODULE bindings.cpp) + 一堆 pybind11 专属配置
pybind11_add_module(${target_name} bindings.cpp)


# target_link_libraries 将库链接到目标
# base_math_lib  是 INTERFACE 库（纯头文件），链接它只是为了继承头文件路径
# static_class_lib 是 STATIC 库，代码会被直接打包进 DemoUtils.so
# utils_lib      是 SHARED 库，运行时动态加载，DemoUtils.so 只记录对它的引用
target_link_libraries(${target_name} PRIVATE base_math_lib static_class_lib utils_lib)


# 模块头文件路径
target_include_directories(${target_name} PRIVATE

    # vcpkg 安装后的标准头文件路径
    "${CMAKE_SOURCE_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/include"

    # ${CMAKE_SOURCE_DIR} 是 CMake 项目根目录
    # ${CMAKE_CURRENT_SOURCE_DIR} 是当前正在处理的目录
    # ${PROJECT_SOURCE_DIR} 最近一个 project() 命令所在的目录

    # ${VCPKG_TARGET_TRIPLET} 是 vcpkg 安装时指定的 triplet，例如 x64-linux
)


# 动态库的运行时搜索路径（RPATH）配置
# 问题背景：
# DemoUtils.so 在运行时需要找到 libutils_lib.dylib
# 默认情况下系统只在 /usr/lib 等标准路径查找，找不到就报错
# RPATH 是嵌入在 .so 文件里的路径提示，告诉系统去哪里找依赖的动态库
#
# $ORIGIN（Linux）/ @loader_path（macOS）：
# 表示"当前这个 .so 文件所在的目录"
# 这样无论项目装到哪里，只要 libutils_lib 和 DemoUtils.so 在同一目录，就能找到
set_target_properties(${target_name} PROPERTIES

    # macOS 用 @loader_path，Linux 用 $ORIGIN
    INSTALL_RPATH "@loader_path"

    # 构建阶段也生效（不设置的话只在 install 后才生效）
    BUILD_WITH_INSTALL_RPATH TRUE
)


# 安装阶段：把 DemoUtils.so 装到目标位置
#[[
TARGETS ${minimal_target}：
    告诉 CMake 你要处理的对象是 ${minimal_target} 这个目标。在你的配置中，它就是 pybind11 生成那个 .so（Linux）或 .pyd（Windows）文件。

DESTINATION：
    关键字，后面跟着的是目标路径。

. (点号)：
    代表相对路径。这个“点”是指相对于 CMAKE_INSTALL_PREFIX 的根目录。
    实际路径 = ${CMAKE_INSTALL_PREFIX} + DESTINATION 指定的路径
]]
# DESTINATION . 是 scikit-build-core 的约定，表示装到 Python 包的根目录
install(TARGETS ${target_name} DESTINATION .)

# 动态库也需要一起安装，否则运行时找不到 libutils_lib.dylib
# 安装到同一目录，配合上面的 RPATH @loader_path 就能被正确找到
install(TARGETS utils_lib DESTINATION .)