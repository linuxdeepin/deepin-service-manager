
#include <QDBusConnection>
#include "service.h"

extern "C" int DSMRegisterObject(const char *name, void *data)
{
    (void)data;
    QDBusConnection::RegisterOptions opts =
        QDBusConnection::ExportAllSlots | QDBusConnection::ExportAllSignals |
        QDBusConnection::ExportAllProperties;

    QDBusConnection::connectToBus(QDBusConnection::SystemBus, QString(name))
        .registerObject("/org/deepin/services/demo1", new Service(), opts);
    return 0;
}
