// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "service.h"
#include "ipwatchd.h"
#include <systemd/sd-bus-vtable.h>

#include <errno.h>
#include <stdio.h>

static sd_bus *dbus = NULL;
sd_bus_slot *slot = NULL;
static char *dbus_path = "/org/deepin/dde/IPWatchD1";
static char *dbus_interface = "org.deepin.dde.IPWatchD1";
static const sd_bus_vtable ipwatchd_vtable[] = {
    SD_BUS_VTABLE_START(0),
    SD_BUS_METHOD("RequestIPConflictCheck", "ss","s",ipwd_conflict_check, SD_BUS_VTABLE_UNPRIVILEGED),
    SD_BUS_SIGNAL("IPConflict","sss",0),
    SD_BUS_SIGNAL("IPConflictReslove","sss",0),
    SD_BUS_VTABLE_END
};

void service_set_dbus(sd_bus *bus)
{
    dbus = bus;
}

sd_bus* service_get_dbus()
{
    return dbus;
}

int dbus_add_object()
{

    int ret = sd_bus_add_object_vtable(dbus, &slot, dbus_path, dbus_interface, ipwatchd_vtable, NULL);
    if (ret < 0) {
        ipwd_message (IPWD_MSG_TYPE_ERROR, "Failed to issue method call: %s\n", strerror(-ret));
        return -1;
    }
    ipwd_message (IPWD_MSG_TYPE_DEBUG, "register ipwatchd plugin success\n");
    return 0;
}

void emit_is_conflict(const char *ip, const char *smac, const char *dmac, int is_conflict)
{
    sd_bus_emit_signal(dbus,
                       dbus_path,
                       dbus_interface,
                       is_conflict ? "IPConflict" : "IPConflictReslove",
                       "sss",
                       ip,
                       smac,
                       dmac);
}

int sd_bus_message_get_data(sd_bus_message *msg, ...) {
    va_list ap;
    va_start(ap, msg);
    int r = sd_bus_message_get_datav(msg, ap);
    va_end(ap);
    return r;
}

int sd_bus_message_get_datav(sd_bus_message *msg, va_list ap) {
    char type = 0;
    const char *contents = NULL;
    int r = 0;
    for (;;) {
        r = sd_bus_message_peek_type(msg, &type, &contents);
        if (r < 0)
            return r;
        switch (type) {
        case SD_BUS_TYPE_STRING: {
            char **s = va_arg(ap, char **);
            r = sd_bus_message_read_basic(msg, type, s);
            if (r < 0)
                return r;
            break;
        }

        case SD_BUS_TYPE_BOOLEAN: {
            bool *b = va_arg(ap, bool *);

            r = sd_bus_message_read_basic(msg, type, b);
            if (r < 0)
                return r;
            break;
        }

        case SD_BUS_TYPE_INT64: {
            int64_t *i64 = va_arg(ap, int64_t *);

            r = sd_bus_message_read_basic(msg, type, i64);
            if (r < 0)
                return r;
            break;
        }

        case SD_BUS_TYPE_INT32: {
            int32_t *i = va_arg(ap, int32_t *);

            r = sd_bus_message_read_basic(msg, type, i);
            if (r < 0)
                return r;
            break;
        }

        case SD_BUS_TYPE_UINT32: {
            uint32_t *i = va_arg(ap, uint32_t *);

            r = sd_bus_message_read_basic(msg, type, i);
            if (r < 0)
                return r;
            break;
        }

        case SD_BUS_TYPE_OBJECT_PATH: {
            char **p = va_arg(ap, char **);

            r = sd_bus_message_read_basic(msg, type, p);
            if (r < 0)
                return r;
            break;
        }

        case SD_BUS_TYPE_ARRAY:
            if (strcmp(contents, "s") == 0) {
                char ***str = va_arg(ap, char ***);
                r = sd_bus_message_read_strv(msg, str);
                if (r < 0)
                    return r;
            } else if (strcmp(contents, "{sv}") == 0) {
                GHashTable **map = va_arg(ap, GHashTable **);
                r = sd_bus_read_dict(msg, map);
                if (r < 0)
                    return r;
            }

            break;
        case _SD_BUS_TYPE_INVALID:
            return r;
        default:
            ipwd_message (IPWD_MSG_TYPE_DEBUG, "get type:%c contents:%s not define,TODO...", type,
                         contents);
            return r;
        }
    }
}