#!/bin/bash
#pushd .
OPTIND=1

BUILD_PYTHON="false"

while getopts ":p" opt; do
  case $opt in
    p)
      BUILD_PYTHON="true"
      echo "Build script will build Python shared libraries additionally!"
      ;;
    \?)
      echo -e "\033[31m Option error ! ! ! \033[0m" >&2
      echo -e "\033[31m Use the -p option to build Python shared libraries additionally. \033[0m" >&2
      echo "Use default option." 
      ;;
  esac
done

#-----这些字段仅适用于安卓环境下-----------------begin-------------------
SUNNY_ANDROID=true
SUNNY_ANDROID_API_LEVEL=24

#ndk里的android.toolchain.cmake文件里指定的几个变量,这里只挑了几个
SUNNY_ANDROID_TOOLCHAIN=clang
SUNNY_ANDROID_ABI=arm64-v8a
SUNNY_ANDROID_PLATFORM=android-${SUNNY_ANDROID_API_LEVEL}
SUNNY_ANDROID_ARM_NEON=TRUE

#编译器
ARM_HOST=aarch64-linux-android
SUNNY_CC=${ARM_HOST}-clang
SUNNY_CXX=${ARM_HOST}-clang++
SUNNY_AR=${ARM_HOST}-ar
#-----这些字段仅适用于安卓环境下-----------------end-------------------
#export PATH=/xxx/xxx:$PATH
#SUNNY_CMAKE_TOOLCHAIN_FILE=/xxx/xxx/xxx.cmake

ORIGIN_DIR=$(pwd)

PROJECT_DIR=$(pwd)/sunny_camera
echo ${PROJECT_DIR}
SDK_ROOT_DIR=${PROJECT_DIR}
SDK_BUILD_DIR=${SDK_ROOT_DIR}/build
if [ ! -d ${SDK_BUILD_DIR} ];then
    mkdir -p ${SDK_BUILD_DIR}
else
    rm -r -f ${SDK_BUILD_DIR}
    mkdir -p ${SDK_BUILD_DIR}
fi

cd ${SDK_BUILD_DIR}
#cmake -DCMAKE_TOOLCHAIN_FILE=${SUNNY_CMAKE_TOOLCHAIN_FILE} -DSUNNY_ANDROID=${SUNNY_ANDROID} -DCMAKE_ANDROID_NDK_TOOLCHAIN_VERSION=${SUNNY_ANDROID_TOOLCHAIN} -DANDROID_TOOLCHAIN=${SUNNY_ANDROID_TOOLCHAIN} -DANDROID_ABI=${SUNNY_ANDROID_ABI} -DANDROID_PLATFORM=${SUNNY_ANDROID_PLATFORM} -DANDROID_ARM_NEON=${SUNNY_ANDROID_ARM_NEON} ..
#cmake -DCMAKE_TOOLCHAIN_FILE=${SUNNY_CMAKE_TOOLCHAIN_FILE} ..
if [ "$BUILD_PYTHON" = "true" ]; then
    echo "BUILD_PYTHON = true"
    PYTHON_ENV=$(which python)
    PYTHON_VERSION="$(python -c "import sys; print('.'.join(sys.version.split('.')[:2]))")"
    PYTHON_INCLUDE="$(echo "$(which python)" | awk -F/bin/python '{print $1}')/include/python${PYTHON_VERSION}"
    PYTHON_LIBRARIES="$(echo "$(which python)" | awk -F/bin/python '{print $1}')/lib"

    cmake -DBUILD_PYTHON=1 -DPYTHON_ENV=${PYTHON_ENV} -DPYTHON_VERSION=${PYTHON_VERSION} -DPYTHON_LIBRARIES=${PYTHON_LIBRARIES} -DPYTHON_INCLUDE=${PYTHON_INCLUDE} ..
else 
    echo "BUILD_PYTHON = flase"
    cmake .. 
fi

# 判断构建是否成功
if [ $? -eq 0 ]; then
    # 构建成功
    make -j16
    if [ $? -eq 0 ]; then
        # 编译成功
        cd ${ORIGIN_DIR}

        LIB_PATH=/usr/local/lib

        # 检查 .bashrc 文件是否已经包含 SUNNY_CAMERA_LIBRARY_PATH 的设置
        if grep -qF "export LD_LIBRARY_PATH=${LIB_PATH}" ~/.bashrc; then
            echo "LD_LIBRARY_PATH is already set in .bashrc."
        else
            # 在 .bashrc 文件末尾添加 SUNNY_CAMERA_LIBRARY_PATH 设置
            echo "export LD_LIBRARY_PATH=${LIB_PATH}:\$LD_LIBRARY_PATH" >> ~/.bashrc
            echo "LD_LIBRARY_PATH added to .bashrc."
        fi

        export LD_LIBRARY_PATH=${LIB_PATH}:$LD_LIBRARY_PATH
        
        echo "-----------------------build success--------------------------"
    else
        cd ${ORIGIN_DIR}
        # 编译失败
        echo "----------build failed, comfirm the requirements satisfied ----------"
    fi
else
    cd ${ORIGIN_DIR}
    # 构建失败
    echo "----------cmake failed, comfirm the requirements satisfied ----------"
fi
