
#include "service.h"

// extern "C" void *ServiceObject(const char *path, const int len)
// {
//     return (void*)calculator_vtable;
// }

extern "C" int DSMRegisterObject(const char *name, void *data)
{
    if (!data) {
        return -1;
    }
    sd_bus *bus = (sd_bus *)data;
    sd_bus_slot *slot = NULL;
    if (sd_bus_add_object_vtable(bus, &slot,
                                    "/org/deepin/services/sdbus/demo1",
                                    "org.deepin.services.sdbus.demo1",
                                    calculator_vtable,
                                    NULL) < 0) {
        return -1;
    }
    return 0;
}