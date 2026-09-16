# SPDX-FileCopyrightText: 2023 - 2026 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: LGPL-3.0-or-later
set(LIB_NAME "core-service")

project(${LIB_NAME})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
add_compile_options(-fPIC)

find_package(Qt${QT_VERSION_MAJOR} ${REQUIRED_QT_VERSION} REQUIRED COMPONENTS Core DBus)

find_package(PkgConfig REQUIRED)
pkg_check_modules(Systemd REQUIRED IMPORTED_TARGET libsystemd) # sdbus

file(GLOB_RECURSE LIB_SRCS
  "*.h"
  "*.cpp"
)
if(NOT ENABLE_RUST)
  list(FILTER LIB_SRCS EXCLUDE REGEX "/service/(rustpluginabi|servicerust)\\.(h|cpp)$")
endif()
add_library(${LIB_NAME} STATIC
  ${LIB_SRCS}
)
target_compile_definitions(${LIB_NAME} PUBLIC
  DSM_ENABLE_RUST=$<BOOL:${ENABLE_RUST}>
)
target_include_directories(${LIB_NAME} PUBLIC
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::DBus
  PkgConfig::Systemd # sdbus
)

target_link_libraries(${LIB_NAME} PRIVATE
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::DBus
  PkgConfig::Systemd # sdbus
)

