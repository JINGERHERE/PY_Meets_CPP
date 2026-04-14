#!/bin/bash

cd "$(dirname "$0")"

# vcpkg 相关命令
#   vcpkg new --application # 创建一个新的应用包
#   vcpkg add port pybind11 # 添加 pybind11 库

echo ">>> DO: Install C++ Dependencies"
vcpkg install || { echo ">>> vcpkg install failed!"; exit 1; }