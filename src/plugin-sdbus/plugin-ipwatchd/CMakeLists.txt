# SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: LGPL-3.0-or-later
cmake_minimum_required(VERSION 3.13)

set(Plugin_NAME "plugin-ipwatchd")

project(${Plugin_NAME})

find_package(PkgConfig REQUIRED)
find_package(Qt5 COMPONENTS LinguistTools REQUIRED)
pkg_check_modules(Systemd REQUIRED IMPORTED_TARGET libsystemd)
pkg_check_modules(GLIB REQUIRED glib-2.0)
pkg_check_modules(PCAP REQUIRED libpcap)
find_library(NET_LIB net)

include_directories(${GLIB_INCLUDE_DIRS})
link_directories(${GLIB_LIBRARY_DIRS})
add_definitions(${GLIB_CFLAGS_OTHER})
include_directories(${PCAP_INCLUDE_DIRS})
link_directories(${PCAP_LIBRARY_DIRS})

# Sources files
file(GLOB_RECURSE SRCS "*.c" "*.h")

add_library(${Plugin_NAME} MODULE
    ${SRCS}
)

target_include_directories(${Plugin_NAME} PUBLIC
    PkgConfig::Systemd
)

target_link_libraries(${Plugin_NAME}
    PkgConfig::Systemd
    pthread
    ${PCAP_LIBRARIES}
    ${NET_LIB}
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug") # delete this judgement in your project
    EXECUTE_PROCESS(COMMAND mkdir -p ${CMAKE_BINARY_DIR}/share/system/)
    EXECUTE_PROCESS(COMMAND cp -f ${CMAKE_CURRENT_SOURCE_DIR}/plugin-ipwatchd.json ${CMAKE_BINARY_DIR}/share/system/)
    target_compile_definitions(${Plugin_NAME} PRIVATE _DEBUG)
else()
    # just need this in your project
    install(TARGETS ${Plugin_NAME} DESTINATION ${CMAKE_INSTALL_LIBDIR}/deepin-service-manager/)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/plugin-ipwatchd.json DESTINATION share/deepin-service-manager/system/)
endif()

install(FILES org.deepin.ipwatchd.conf
    DESTINATION share/dbus-1/system.d/
)