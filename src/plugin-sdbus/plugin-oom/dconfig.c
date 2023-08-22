// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
#include "dconfig.h"

#include "3rdparty/earlyoom/msg.h"

#include <systemd/sd-bus.h>
#include <systemd/sd-event.h>

void get_config_value(
    sd_bus *bus, const char *path, const char *key, const char *outtype, void *output);
int match_handler(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);

void parse_config(sd_bus *bus, poll_loop_args_t *args)
{
    sd_bus_error error = SD_BUS_ERROR_NULL;
    sd_bus_message *m = NULL;
    const char *path;
    int r;
    /* Issue the method call and store the respons message in m */
    r = sd_bus_call_method(bus,
                           "org.desktopspec.ConfigManager", /* service to contact */
                           "/",                             /* object path */
                           "org.desktopspec.ConfigManager", /* interface name */
                           "acquireManager",                /* method name */
                           &error,                          /* object to return error in */
                           &m,                              /* return message on success */
                           "sss",                           /* input signature */
                           "org.deepin.oom",                /* first argument */
                           "org.deepin.oom",                /* second argument */
                           "");
    if (r < 0) {
        warn("Failed to issue method call: %s\n", error.message);
        goto finish;
    }

    /* Parse the response message */
    r = sd_bus_message_read(m, "o", &path);
    if (r < 0) {
        warn("Failed to parse response message: %s\n", strerror(-r));
        goto finish;
    }

    /* Get config value */
    get_config_value(bus, path, "mem_term_percent", "x", &args->mem_term_percent);
    get_config_value(bus, path, "mem_kill_percent", "x", &args->mem_kill_percent);
    get_config_value(bus, path, "swap_term_percent", "x", &args->swap_term_percent);
    get_config_value(bus, path, "swap_kill_percent", "x", &args->swap_kill_percent);
    get_config_value(bus, path, "report_interval_ms", "x", &args->report_interval_ms);
    get_config_value(bus, path, "kill_process_group", "b", &args->kill_process_group);
    get_config_value(bus, path, "ignore_root_user", "b", &args->ignore_root_user);
    get_config_value(bus, path, "prefer_regex", "s", &args->prefer_regex);
    get_config_value(bus, path, "avoid_regex", "s", &args->avoid_regex);
    get_config_value(bus, path, "ignore_regex", "s", &args->ignore_regex);

    /* connect signal */
    r = sd_bus_match_signal(bus,
                            NULL,
                            "org.desktopspec.ConfigManager",
                            path,
                            "org.desktopspec.ConfigManager.Manager",
                            "valueChanged",
                            match_handler,
                            args);

finish:
    sd_bus_error_free(&error);
    sd_bus_message_unref(m);
}

void get_config_value(
    sd_bus *bus, const char *path, const char *key, const char *outtype, void *output)
{
    sd_bus_error error = SD_BUS_ERROR_NULL;
    sd_bus_message *m = NULL;
    int r;
    r = sd_bus_call_method(bus,
                           "org.desktopspec.ConfigManager",         /* service to contact */
                           path,                                    /* object path */
                           "org.desktopspec.ConfigManager.Manager", /* interface name */
                           "value",                                 /* method name */
                           &error,                                  /* object to return error in */
                           &m,                                      /* return message on success */
                           "s",                                     /* input signature */
                           key);
    if (r < 0) {
        warn("Failed to issue method call: %s\n", error.message);
        goto finish;
    }

    /* Parse the response message */
    r = sd_bus_message_read(m, "v", outtype, output);

    if (r < 0) {
        warn("Failed to parse response message: %s\n", strerror(-r));
        goto finish;
    }
finish:
    sd_bus_error_free(&error);
    sd_bus_message_unref(m);
}

typedef struct
{
    char *name;
    char *type;
    void *value;
} config_value_t;

int match_handler(sd_bus_message *m, void *userdata, sd_bus_error *ret_error)
{
    poll_loop_args_t *args = userdata;
    const char *value;
    sd_bus *bus = sd_bus_message_get_bus(m);
    const char *path = sd_bus_message_get_path(m);
    const config_value_t configs[] = {
        { "mem_term_percent", "x", &args->mem_term_percent },
        { "mem_kill_percent", "x", &args->mem_kill_percent },
        { "swap_term_percent", "x", &args->swap_term_percent },
        { "swap_kill_percent", "x", &args->swap_kill_percent },
        { "report_interval_ms", "x", &args->report_interval_ms },
        { "kill_process_group", "b", &args->kill_process_group },
        { "ignore_root_user", "b", &args->ignore_root_user },
        { "prefer_regex", "s", &args->prefer_regex },
        { "avoid_regex", "s", &args->avoid_regex },
        { "ignore_regex", "s", &args->ignore_regex },
    };
    int r;
    r = sd_bus_message_read(m, "s", &value);
    if (r < 0) {
        warn("Failed to parse response message: %s\n", strerror(-r));
        return r;
    }
    for (int i = 0; i < sizeof(configs) / sizeof(config_value_t); i++) {
        if (strcmp(configs[i].name, value) == 0) {
            get_config_value(bus, path, value, configs[i].type, configs[i].value);
            printf("update config %s = %d\n", configs[i].name, *(int *)configs[i].value);
            break;
        }
    }

    return 0;
}
