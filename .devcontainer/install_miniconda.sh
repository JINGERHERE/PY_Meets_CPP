#!/bin/bash
set -e # 错误时退出整个脚本

# 自动检测当前 CPU 架构
ARCH=$(uname -m)
case "$ARCH" in
    x86_64)
        CONDA_ARCH="x86_64"
        ;;
    aarch64|arm64)
        CONDA_ARCH="aarch64"
        ;;
    armv7l)
        CONDA_ARCH="armv7l"
        ;;
    *)
        echo "Unsupported architecture: $ARCH"
        exit 1
        ;;
esac

MINICONDA_VERSION="latest"
MINICONDA_URL="https://mirrors.tuna.tsinghua.edu.cn/anaconda/miniconda/Miniconda3-${MINICONDA_VERSION}-Linux-${CONDA_ARCH}.sh"

# 下载 Miniconda 到 /tmp/miniconda.sh
wget "$MINICONDA_URL" -O /tmp/miniconda.sh
# 执行 Miniconda 安装脚本（-b 表示静默安装）
# -p 表示安装到 /opt/conda 目录 -u 表示安装完成后自动初始化 shell
/bin/bash /tmp/miniconda.sh -b -p /opt/miniconda -u
# 删除 miniconda 安装脚本
rm /tmp/miniconda.sh

# 执行初始化 conda 的初始化脚本（-u 已做，但加一行无害）
/opt/miniconda/bin/conda init bash