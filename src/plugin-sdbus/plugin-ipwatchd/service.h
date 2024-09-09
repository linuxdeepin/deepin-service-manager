// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef DEEPIN_SERVICE_SERVICE_H
#define DEEPIN_SERVICE_SERVICE_H
#include <systemd/sd-bus-vtable.h>
#include <glib.h>
#include <systemd/sd-bus.h>
#include <stdbool.h>

#define DLL_LOCAL __attribute__((visibility("hidden")))
DLL_LOCAL void service_set_dbus(sd_bus *bus);
sd_bus* service_get_dbus();
DLL_LOCAL int dbus_add_object(void);

int sd_bus_message_get_datav(sd_bus_message *msg, va_list ap);
int sd_bus_message_get_data(sd_bus_message *msg, ...);
int sd_bus_read_dict(sd_bus_message *msg, GHashTable **map);
void emit_is_conflict(const char *ip, const char *smac, const char *dmac, int is_conflict);

#endif // DEEPIN_SERVICE_SERVICE_H
