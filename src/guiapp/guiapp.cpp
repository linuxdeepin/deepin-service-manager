// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//
// GUI 应用桥接 .so
//
// 设计目的:让 deepin-service-manager 主二进制不再直接链接 libQt6Gui.so.6,
// 从而在不走 GUI 分支时(QCoreApplication 路径)整个进程不加载 Qt6Gui 传递闭包,
// 节省内存。仅当某组插件需要 GUI(dde 组)时,host 才通过 dlopen 加载本 .so,
// 由本 .so 构造 QGuiApplication 并返回 C++ 基类指针给 host。
//
// 约束:
//  - 一个进程只能有一个 QCoreApplication 派生对象,本函数构造的就是那个唯一对象。
//  - QGuiApplication 必须是进程里第一个 QApp 对象,host 在调用本函数前不会 new 任何 QApp。
//  - 返回 QCoreApplication* :host 只用基类 API;GUI 插件通过 qGuiApp/qApp 仍能取到它。

#include <QGuiApplication>

// C 接口:host 用函数指针调用,自身无需 link Qt6Gui、无需 QGuiApplication 头文件
extern "C" QCoreApplication *dsm_createGuiApplication(int &argc, char **argv)
{
    return new QGuiApplication(argc, argv);
}
