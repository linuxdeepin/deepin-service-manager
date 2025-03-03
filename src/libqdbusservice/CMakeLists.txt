# SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: LGPL-3.0-or-later
set(LIB_NAME "deepin-qdbus-service")

project(${LIB_NAME})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt${QT_VERSION_MAJOR} ${REQUIRED_QT_VERSION} REQUIRED COMPONENTS Core DBus)

file(GLOB_RECURSE LIB_SRCS
  "*.h"
  "*.cpp"
)
add_library(${LIB_NAME} SHARED
  ${LIB_SRCS}
)

target_link_directories(${LIB_NAME} PRIVATE
  ${CMAKE_BINARY_DIR}/core/
)

target_include_directories(${LIB_NAME} PUBLIC
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::DBus
  ${SRC_BASE_DIR}/core
)

target_link_libraries(${LIB_NAME} PRIVATE
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::DBus
  core-service
)
ADD_DEFINITIONS(-DEXPORT_LIBRARY)

install(TARGETS ${LIB_NAME} DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES qdbusservice.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${LIB_NAME})
