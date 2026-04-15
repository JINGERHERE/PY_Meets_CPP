# ./cpp/static_class/CMakeLists.txt


# 生成 static_class_lib 库
add_library(static_class_lib STATIC
    logger.cpp
)

# 包含头文件路径
target_include_directories(static_class_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
