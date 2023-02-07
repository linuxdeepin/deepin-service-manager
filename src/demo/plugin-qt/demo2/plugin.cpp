#include <QDBusConnection>
#include <QDebug>

#include "demo2aadaptor.h"
#include "service.h"

static Service *service = nullptr;

extern "C" int DSMRegister(const char *name, void *data)
{
    (void)data;
    service = new Service();
    new Demo2aAdaptor(service);
    QDBusConnection::connectToBus(QDBusConnection::SessionBus, QString(name))
        .registerObject("/org/deepin/service/demo2",
                        service,
                        QDBusConnection::ExportAdaptors);
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
