#!/bin/bash

pip uninstall sunny_camera -y 

PROJECT_DIR=$(pwd)/sunny_camera
echo ${PROJECT_DIR}
SDK_ROOT_DIR=${PROJECT_DIR}
SDK_BUILD_DIR=${SDK_ROOT_DIR}/build
rm -rf ${SDK_BUILD_DIR}

LIB_PATH=/usr/local/lib
rm -f ${LIB_PATH}/libtof_dev_sdk.so

if [ $? -eq 0 ]; then
    echo "---------------uninstall success---------------"
    echo "libtof_dev_sdk.so is already uninstalled from ${LIB_PATH}."

else
    # 卸载失败
    echo "----------uninstall failed, please use sudo command----------"
fi