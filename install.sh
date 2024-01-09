#!/bin/bash

PROJECT_DIR=$(pwd)/sunny_camera
echo ${PROJECT_DIR}

SDK_ROOT_DIR=${PROJECT_DIR}
SDK_BUILD_DIR=${SDK_ROOT_DIR}/build

# 检查构建目录是否存在
if [ ! -d ${SDK_BUILD_DIR} ]; then
    echo "Please build first!!!"
    exit 1
fi

cd ${SDK_BUILD_DIR}

# 安装 SDK
make install

# 检查 make install 命令是否成功
if [ $? -eq 0 ]; then
    echo "---------------------install success----------------------"
    echo "sunny_camera is already installed to /usr/local."
else
    # 安装失败
    echo "----------install failed, please try rebuild----------"
    exit 1
fi
