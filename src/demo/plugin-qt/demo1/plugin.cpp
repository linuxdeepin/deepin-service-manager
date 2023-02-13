
#include "service.h"

#include <QDBusConnection>

static Service *service = nullptr;

extern "C" int DSMRegister(const char *name, void *data)
{
    (void)data;
    service = new Service();
    QDBusConnection::RegisterOptions opts = QDBusConnection::ExportAllSlots
            | QDBusConnection::ExportAllSignals | QDBusConnection::ExportAllProperties;

    QDBusConnection::connectToBus(QDBusConnection::SystemBus, QString(name))
            .registerObject("/org/deepin/services/demo1", service, opts);
    return 0;
}

// 该函数用于资源释放
// 非常驻插件必须实现该函数，以防内存泄漏
extern "C" int DSMUnRegister(const char *name, void *data)
{
    (void)name;
    (void)data;
    service->deleteLater();
    service = nullptr;
    return 0;
}
