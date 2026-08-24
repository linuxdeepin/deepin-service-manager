# SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: LGPL-3.0-or-later
#
# GUI 应用桥接库:link Qt6Gui,导出 C 接口供 host dlopen 调用。
# host 主二进制不 link 本库(用 QLibrary 运行时加载),以保证 host 的
# DT_NEEDED 不含 libQt6Gui.so.6。

set(CMAKE_AUTOMOC ON)

find_package(Qt${QT_VERSION_MAJOR} ${REQUIRED_QT_VERSION} REQUIRED COMPONENTS Gui Core)

add_library(dsm-guiapp SHARED guiapp.cpp)

target_link_libraries(dsm-guiapp PRIVATE
  Qt${QT_VERSION_MAJOR}::Gui
  Qt${QT_VERSION_MAJOR}::Core
)

install(TARGETS dsm-guiapp DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR})
