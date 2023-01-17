
#include "service.h"

extern "C" int DSMRegisterObject(const char *name, void *data)
{
    (void)name;
    if (!data) {
        return -1;
    }
    sd_bus *bus = (sd_bus *)data;
    sd_bus_slot *slot = NULL;
    if (sd_bus_add_object_vtable(bus, &slot,
                                    "/org/deepin/service/sdbus/demo1",
                                    "org.deepin.service.sdbus.demo1",
                                    calculator_vtable,
                                    NULL) < 0) {
        return -1;
    }
    return 0;
}
