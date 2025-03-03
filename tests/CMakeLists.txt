# SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: LGPL-3.0-or-later
cmake_minimum_required(VERSION 3.16)

set(BIN_NAME dde_service_manager_ut)

# 自动生成moc文件
set(CMAKE_AUTOMOC ON)

# 源文件
file(GLOB_RECURSE SRCS
    "../src/deepin-service-manager/dbus_manager.h"
    "../src/deepin-service-manager/dbus_manager.cpp"
    "*.h"
    "*.cpp")

# Sources files
file(GLOB_RECURSE PLUGIN_SRCS "")

# 查找gmock的cmake文件
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/tests/cmake/modules)

# 查找依赖库
find_package(PkgConfig REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} ${REQUIRED_QT_VERSION} REQUIRED COMPONENTS Widgets DBus Test)
find_package(GTest REQUIRED)
find_package(GMock REQUIRED)

ADD_COMPILE_OPTIONS(-fno-access-control)

# 添加执行文件信息
add_executable(${BIN_NAME}
    ${SRCS}
    ${PLUGIN_SRCS})

# 包含路径
target_include_directories(${BIN_NAME} PUBLIC
    ../src/dde-service-manager
)

# 链接库
target_link_libraries(${BIN_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Test
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::DBus
    ${GTEST_LIBRARIES}
    ${GMOCK_LIBRARIES}
    -lpthread
    -lm
)

add_custom_target(check)

add_custom_command(TARGET check
    COMMAND ./${BIN_NAME}
    )

add_dependencies(check ${BIN_NAME})
