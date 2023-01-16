cmake_minimum_required(VERSION 3.7)

set (VERSION "1.0.0" CACHE STRING "define project version")

project(deepin-service)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX /usr)
endif()

include(GNUInstallDirs)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wall -Wextra")
# GNU 默认
set(SAFE_COMPILER "-fstack-protector-strong -D_FORTITY_SOURCE=1 -z noexecstack -pie -z lazy")
set(UT_COMPILER "-fprofile-arcs -ftest-coverage")

# Clang
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(SAFE_COMPILER "-fstack-protector-strong -D_FORTITY_SOURCE=1")
    set(UT_COMPILER "-fprofile-instr-generate -ftest-coverage")
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${UT_COMPILER}")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SAFE_COMPILER}")
endif()

# 设置二进制输出目录 方便调试
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib")

add_subdirectory("src")
# TODO
add_subdirectory("misc")

# TODO:tests, 目前的tests是以前的，后续删掉，补充新的test
# add_subdirectory("tests")

