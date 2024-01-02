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

# 检查 make 命令是否存在
if ! command -v make &> /dev/null; then
    echo "Make command not found. Please install make and try again."
    exit 1
fi

# 安装 SDK
make install

LIB_PATH=/usr/local/lib
cp ${SDK_ROOT_DIR}/libs/libtof_dev_sdk.so ${LIB_PATH}

# 检查 make install 命令是否成功
if [ $? -eq 0 ]; then
    echo "---------------------install success----------------------"
    echo "libtof_dev_sdk.so is already installed to ${LIB_PATH}."
else
    # 安装失败
    echo "----------install failed, please use sudo command----------"
    exit 1
fi
