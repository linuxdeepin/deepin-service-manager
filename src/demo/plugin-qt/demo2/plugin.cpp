#include <QtCore/qglobal.h>
#include <QDebug>
#include <QDBusConnection>

#include "service.h"
#include "demo2aadaptor.h"

extern "C" int DSMRegisterObject(const char *name, void *data)
{
    (void)data;
    Service *srv = new Service();
    new Demo2aAdaptor(srv);
    QDBusConnection::connectToBus(QDBusConnection::SessionBus, QString(name)).registerObject("/org/deepin/services/demo2", srv, QDBusConnection::ExportAdaptors);
    return 0;
}
