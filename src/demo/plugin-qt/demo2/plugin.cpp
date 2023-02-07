#include <QtCore/qglobal.h>
#include <QDBusConnection>
#include <QDebug>

#include "demo2aadaptor.h"
#include "service.h"

extern "C" int DSMRegisterObject(const char *name, void *data)
{
    (void)data;
    Service *srv = new Service();
    new Demo2aAdaptor(srv);
    QDBusConnection::connectToBus(QDBusConnection::SessionBus, QString(name))
        .registerObject(
            "/org/deepin/service/demo2", srv, QDBusConnection::ExportAdaptors);
    return 0;
}
