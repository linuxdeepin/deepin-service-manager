# SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: LGPL-3.0-or-later
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt${QT_VERSION_MAJOR} ${REQUIRED_QT_VERSION} REQUIRED COMPONENTS Core DBus)

set(SERVER_NAME "demo-server")
add_executable(${SERVER_NAME}
    server.cpp
    server1.h
    server2.h
)

set(CLIENT_NAME "demo-client")
add_executable(${CLIENT_NAME}
    client.cpp
)

target_link_libraries(${SERVER_NAME} PRIVATE
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::DBus
)


target_link_libraries(${CLIENT_NAME} PRIVATE
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::DBus
)

ADD_DEFINITIONS(-DNEW_QT)
