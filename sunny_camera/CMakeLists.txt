# -*- coding: utf-8 -*-
cmake_minimum_required(VERSION 3.12)
project(SunnyCamera VERSION 1.0.0)

# set(CMAKE_BUILD_TYPE Debug)

# 默认构建类型为 Release
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(LIBS_DIR ${CMAKE_SOURCE_DIR}/libs)
set(PARAM_DIR ${CMAKE_SOURCE_DIR}/parameter)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

# 使用 include(GNUInstallDirs) 引入标准目录变量
include(GNUInstallDirs)
include_directories(${INCLUDE_DIR})
link_directories(${LIBS_DIR})

if(DEFINED BUILD_PYTHON)
  set(PYBIND11_PYTHON_VERSION ${PYTHON_VERSION})
  # 设置pybind的构建目录
  find_package(pybind11 REQUIRED)
  include_directories(${pybind11_INCLUDE_DIRS})
endif()

# 设置 OpenCV 的构建目录
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# 生成导出头文件
include(GenerateExportHeader)

# 添加源文件
add_library(SunnyCamera SHARED ${CMAKE_SOURCE_DIR}/src/sunny_camera.cpp ${CMAKE_SOURCE_DIR}/src/camera.cpp)

if(DEFINED BUILD_PYTHON)
  add_library(SunnyCameraPython SHARED ${CMAKE_SOURCE_DIR}/src/sunny_camera.cpp ${CMAKE_SOURCE_DIR}/src/camera.cpp ${CMAKE_SOURCE_DIR}/src/python_interface.cpp)
endif()

# 生成导出头文件
generate_export_header(SunnyCamera
  BASE_NAME SunnyCamera
  EXPORT_MACRO_NAME SunnyCamera_EXPORT
  EXPORT_FILE_NAME SunnyCamera_Export.h
  STATIC_DEFINE SunnyCamera_BUILT_AS_STATIC
)

# link python interpreter
if(DEFINED BUILD_PYTHON)
  message(WARNING "\nPlease double check python env && version")
  message(STATUS "**********************!*!*!*!*!**********************")
  message(STATUS "Current python env is : ${PYTHON_ENV}")
  # 输出 Python 包含目录和库目录
  message(STATUS "Python version: python${PYTHON_VERSION}")
  message(STATUS "**********************!*!*!*!*!**********************\n\n")

  # 输出 Python 包含目录和库目录
  message(STATUS "Python version: python${PYTHON_VERSION}")
  message(STATUS "Python libraries: ${PYTHON_LIBRARIES}")
  message(STATUS "Python include directories: ${PYTHON_INCLUDE}")

  target_include_directories(SunnyCamera PRIVATE ${PYTHON_INCLUDE})
  target_link_directories(SunnyCamera PRIVATE ${PYTHON_LIBRARIES})
  target_link_libraries(SunnyCamera python${PYTHON_VERSION})
endif()

# link opencv
target_link_libraries(SunnyCamera ${OpenCV_LIBS})         #SunnyCamera
if(DEFINED BUILD_PYTHON)
  target_link_libraries(SunnyCameraPython ${OpenCV_LIBS})   #SunnyCameraPython

  # link pybind
  target_link_libraries(SunnyCameraPython pybind11::module) #SunnyCameraPython
endif()

add_definitions(-std=c++11)
if (WIN32)
    add_definitions(/wd4430 /wd4819 /wd4305 /wd4244 /wd4018 /wd4800 /wd4267 /wd4101 /wd4099 /wd4251 /wd4200 /wd4065 /wd4996)# Ignore some of the warnings
    add_definitions(-DWIN32 -D_CRT_SECURE_NO_WARNINGS)
    target_link_libraries(SunnyCamera tof_dev_sdk)
    if(DEFINED BUILD_PYTHON)
      target_link_libraries(SunnyCameraPython tof_dev_sdk) 
    endif()
elseif (UNIX)
    add_compile_options(-w)# Ignore all warnings
    add_definitions(-DLINUX)
    if(NOT SUNNY_ANDROID)
        target_link_libraries(SunnyCamera tof_dev_sdk)
        target_link_libraries(SunnyCamera pthread)
        if(DEFINED BUILD_PYTHON)
          target_link_libraries(SunnyCameraPython tof_dev_sdk)
          target_link_libraries(SunnyCameraPython pthread)
        endif()
    else()
        target_link_libraries(SunnyCamera tof_dev_sdk)
        if(DEFINED BUILD_PYTHON)
          target_link_libraries(SunnyCameraPython tof_dev_sdk)
        endif()
    endif()
endif ()

# 安装目标
install(
  TARGETS SunnyCamera
  EXPORT SunnyCamera
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  INCLUDES DESTINATION include
)
install(
  FILES ${LIBS_DIR}/libtof_dev_sdk.so
  DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
if(DEFINED BUILD_PYTHON)
  set_target_properties(SunnyCameraPython PROPERTIES
      OUTPUT_NAME "SunnyCamera"  # 替换为你想要的.so文件名
      PREFIX ""  # 设置为""以去掉前缀，如果不需要的话
  )
  install(
    TARGETS SunnyCameraPython
    LIBRARY DESTINATION ${LIBS_DIR}
  )
endif()

# 安装头文件
install(
  DIRECTORY ${INCLUDE_DIR}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/SunnyCamera
)
message(STATUS "C++ version lib target dir: ${CMAKE_INSTALL_LIBDIR}")

if(DEFINED BUILD_PYTHON)
  message(STATUS "python version lib target dir: ${LIBS_DIR}")
endif()

# 生成 SunnyCameraConfig.cmake 文件
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/SunnyCameraConfig.cmake"
  VERSION ${SunnyCamera_VERSION}
  COMPATIBILITY AnyNewerVersion
)

# 安装 SunnyCameraConfig.cmake 文件
install(
  FILES
    "${CMAKE_CURRENT_BINARY_DIR}/SunnyCameraConfig.cmake"
  DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/SunnyCamera
)
message(STATUS "SunnyCameraConfig.cmake target dir: ${CMAKE_INSTALL_LIBDIR}/cmake/SunnyCamera")

# 安装 SunnyCamera.cmake 文件
install(
  EXPORT SunnyCamera
  FILE SunnyCamera.cmake
  NAMESPACE SunnyCamera::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SunnyCamera
)
message(STATUS "SunnyCamera.cmake target dir: ${CMAKE_INSTALL_LIBDIR}/cmake/SunnyCamera")
