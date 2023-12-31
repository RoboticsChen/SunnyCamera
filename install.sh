#!/bin/bash
PROJECT_DIR=$(pwd)/sunny_camera
echo ${PROJECT_DIR}
SDK_ROOT_DIR=${PROJECT_DIR}
SDK_BUILD_DIR=${SDK_ROOT_DIR}/build
if [ ! -d ${SDK_BUILD_DIR} ];then
    echo "Please build first!!!"
fi

cd ${SDK_BUILD_DIR}

make install

LIB_PATH=/usr/local/lib

cp ${SDK_ROOT_DIR}/libs/libtof_dev_sdk.so ${LIB_PATH}
echo "*.so files is already installed to ${LIB_PATH}."

