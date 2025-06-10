# SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: LGPL-3.0-or-later
cmake_minimum_required(VERSION 3.13)

set(BIN_NAME "OOMScoreAdjust")

project(${BIN_NAME})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

include(GNUInstallDirs)
file(GLOB_RECURSE SRCS "*.h" "*.cpp")

find_package(Qt${QT_VERSION_MAJOR} ${REQUIRED_QT_VERSION} REQUIRED COMPONENTS Core DBus)
find_package(Dtk${DTK_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_library(${BIN_NAME} MODULE
    ${SRCS}
)

target_include_directories(${BIN_NAME} PUBLIC
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::DBus
  ${DtkCore_INCLUDE_DIRS}
)

target_link_libraries(${BIN_NAME} PRIVATE
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::DBus
  Dtk${DTK_VERSION_MAJOR}::Core
)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/OOM-Score-Adjust.json DESTINATION share/deepin-service-manager/system/)
install(FILES org.deepin.service.OOMScoreAdjust.conf DESTINATION share/dbus-1/system.d/)
install(FILES ./configs/oom-score-adjust.desktop DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/xdg/autostart/)
install(TARGETS ${BIN_NAME} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/deepin-service-manager/)
install(FILES ./configs/org.deepin.service.manager.oom-score-adjust.json DESTINATION share/dsg/configs/org.deepin.service.manager/)
install(FILES ./configs/org.deepin.OOMScoreAdjust.service DESTINATION share/dbus-1/system-services/)
