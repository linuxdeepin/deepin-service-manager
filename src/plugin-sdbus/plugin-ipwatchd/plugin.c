// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
#include "service.h"
#include "ipwatchd.h"
#include <pthread.h>
int DSMRegister(const char *name, void *data)
{
    (void)name;
    if (!data) {
        return -1;
    }
    service_set_dbus(data);
    if (dbus_add_object())
        return -1;

    static ipwatchd_args_t args = {
        .config_file = "/etc/ipwatchd.conf",
    };
    pthread_t thread;
    pthread_create(&thread, NULL, (void *)&start_ipwatchd, &args);
    return 0;
}

// 该函数用于资源释放
// 非常驻插件必须实现该函数，以防内存泄漏
int DSMUnRegister(const char *name, void *data)
{
    (void)name;
    (void)data;
    stop_ipwatchd();
    return 0;
}
