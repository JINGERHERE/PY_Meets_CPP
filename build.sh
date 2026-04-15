#!/bin/bash
# build.sh (macOS & Linux)

PROJECT_ROOT="$(cd "$(dirname "$0")" && pwd)"
cd "$PROJECT_ROOT"

# 1. 平台检测 (仅用于 Triplet)
ARCH="$(uname -m)"
if [[ "$OSTYPE" == "darwin"* ]]; then
    [ "$ARCH" = "arm64" ] && TRIPLET="arm64-osx" || TRIPLET="x64-osx"
else
    TRIPLET="x64-linux"
fi

echo "START: Unix Build for $TRIPLET"

# 2. 清理
echo "DO: 删除环境中旧的文件和目录: uv.lock, build, .venv, stubs"
rm -rf uv.lock || { echo ">>> uv.lock not found, ignore"; }
rm -rf build || { echo ">>> build not found, ignore"; }
rm -rf .venv || { echo ">>> .venv not found, ignore"; }
rm -rf stubs || { echo ">>> stubs not found, ignore"; }
echo ""

# 3. 检测 vcpkg
echo "START: 检查 vcpkg ..."
if command -v vcpkg &>/dev/null; then
    VCPKG_EXE="$(command -v vcpkg)"
elif [ -f "$PROJECT_ROOT/vcpkg/vcpkg" ]; then
    VCPKG_EXE="$PROJECT_ROOT/vcpkg/vcpkg"
else
    echo "[ERROR] vcpkg not found. Run bash vcpkg_install.sh first."
    exit 1
fi
echo ""

# 4. 构建与同步
echo "START: 通过 vcpkg 安装 C++ 依赖..."
"$VCPKG_EXE" install --triplet="$TRIPLET" || exit 1

VCPKG_DIR="$(dirname "$VCPKG_EXE")"
export SKBUILD_CMAKE_ARGS="-DCMAKE_TOOLCHAIN_FILE=$VCPKG_DIR/scripts/buildsystems/vcpkg.cmake;-DVCPKG_TARGET_TRIPLET=$TRIPLET"

echo ""

echo "START: 使用 uv sync 构建环境..."
uv cache clean || { echo ">>> uv cache clean failed!"; exit 1; }
uv sync --all-groups || { echo ">>> uv sync failed!"; exit 1; }
echo ""

# 5. 生成存根与测试
echo "START: 生成 .pyi 存根文件"
uv run pybind11-stubgen DemoUtils -o stubs/ || {
    echo "    -> 存根生成失败（不影响运行，可忽略）"
}
echo ""

echo "-------------------------------------------------------"
echo "DONE!"
echo "-------------------------------------------------------"
echo ""

echo "START: 测试运行 Demo (main.py)"
uv run python main.py