# -*- coding: utf-8 -*-
cmake_minimum_required(VERSION 2.8.0)
project(sunny_camera)

# set(CMAKE_BUILD_TYPE Debug)

set(OUTPUT_DIR ${CMAKE_SOURCE_DIR}/bin)
set(LIBS_DIR ${CMAKE_SOURCE_DIR}/libs)
set(PARAM_DIR ${CMAKE_SOURCE_DIR}/parameter)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIR})

set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(LIB_DIR ${LIBS_DIR})

include_directories(${INCLUDE_DIR})
link_directories(${LIB_DIR})

set(PYBIND11_PYTHON_VERSION ${PYTHON_VERSION})
# 设置pybind的构建目录
find_package(pybind11 REQUIRED)
include_directories(${pybind11_INCLUDE_DIRS})

# 设置 OpenCV 的构建目录
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_library(camera SHARED ${CMAKE_SOURCE_DIR}/src/sunny_camera.cpp ${CMAKE_SOURCE_DIR}/src/camera.cpp)

# link python interpreter
if(DEFINED PYTHON_ENV)
    message( "\n**********************************************************************************\n
!!!!!  Be careful Current python env is : ${PYTHON_ENV}    !!!!! \n
**********************************************************************************\n")
endif()

# 输出 Python 包含目录和库目录
message(STATUS "Python version: python${PYTHON_VERSION}")
message(STATUS "Python libraries: ${PYTHON_LIBRARIES}")
message(STATUS "Python include directories: ${PYTHON_INCLUDE}")

target_include_directories(camera PRIVATE ${PYTHON_INCLUDE})
target_link_directories(camera PRIVATE ${PYTHON_LIBRARIES})
target_link_libraries(camera python${PYTHON_VERSION})

# link pybind
target_link_libraries(camera pybind11::module)
# link opencv
target_link_libraries(camera ${OpenCV_LIBS})


add_definitions(-std=c++11)
if (WIN32)
    add_definitions(/wd4430 /wd4819 /wd4305 /wd4244 /wd4018 /wd4800 /wd4267 /wd4101 /wd4099 /wd4251 /wd4200 /wd4065 /wd4996)# Ignore some of the warnings
    add_definitions(-DWIN32 -D_CRT_SECURE_NO_WARNINGS)
    target_link_libraries(camera tof_dev_sdk)
elseif (UNIX)
    add_compile_options(-w)# Ignore all warnings
    add_definitions(-DLINUX)
    if(NOT SUNNY_ANDROID)
        target_link_libraries(camera tof_dev_sdk)
        target_link_libraries(camera pthread)
    else()
        target_link_libraries(camera tof_dev_sdk)
    endif()
endif ()

# install(DIRECTORY "${LIBS_DIR}/" DESTINATION "${OUTPUT_DIR}" PATTERN "*" PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ)
# install(DIRECTORY "${PARAM_DIR}" DESTINATION "${OUTPUT_DIR}")
# 设置生成的.so文件的名称

set_target_properties(camera PROPERTIES
    OUTPUT_NAME "sunny_camera"  # 替换为你想要的.so文件名
    PREFIX ""  # 设置为""以去掉前缀，如果不需要的话
)

install(TARGETS camera DESTINATION ${CMAKE_SOURCE_DIR}/libs)
