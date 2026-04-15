#!/bin/bash
# vcpkg_install.sh (macOS/Linux 专用版)

# 确保脚本即使在子目录运行也能定位到正确的项目根目录
PROJECT_ROOT="$(cd "$(dirname "$0")" && pwd)"

echo ">>> [UNIX] 正在项目根目录准备 vcpkg..."

# 检查并克隆仓库
if [ ! -d "${PROJECT_ROOT}/vcpkg" ]; then
    echo "    -> 未检测到 ./vcpkg 目录，开始 clone (depth=1)..."
    git clone --depth=1 https://github.com/microsoft/vcpkg.git "${PROJECT_ROOT}/vcpkg" || {
        echo " [ERROR] Git clone 失败，请检查 Git 或网络连接。"
        exit 1
    }
else
    echo "    -> vcpkg 目录已存在，跳过克隆。"
fi

echo "    -> 开始引导 (Bootstrap) vcpkg..."
# 赋予执行权限并运行
chmod +x ${PROJECT_ROOT}/vcpkg/bootstrap-vcpkg.sh
# 执行 Bootstrap 引导程序
${PROJECT_ROOT}/vcpkg/bootstrap-vcpkg.sh -disableMetrics

# 结果检查
if [ $? -eq 0 ]; then
    echo "-------------------------------------------------------"
    echo ">>> vcpkg 安装/引导成功！"
    echo "-------------------------------------------------------"
else
    echo " [ERROR] vcpkg 引导失败，请检查上方日志。"
    exit 1
fi