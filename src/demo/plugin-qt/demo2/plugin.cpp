#include <QtCore/qglobal.h>
#include <QDebug>
#include <QDBusConnection>

#include "service.h"
#include "demo2aadaptor.h"

#if defined(EXPORT_LIBRARY)
#  define EXPORT_PLUGIN_LIBRARY Q_DECL_EXPORT
#else
#  define EXPORT_PLUGIN_LIBRARY Q_DECL_IMPORT
#endif

// extern "C" Q_DECL_EXPORT void *ServiceObject(const char *path, const int len)
// {
//     qInfo() << "Plugin export" << QString(path);
//     Service *srv = new Service();
//     new Demo2aAdaptor(srv);
//     return srv;
// }

extern "C" int DSMRegisterObject(const char *name, void *data)
{
    Service *srv = new Service();
    new Demo2aAdaptor(srv);
    QDBusConnection::connectToBus(QDBusConnection::SessionBus, QString(name)).registerObject("/org/deepin/services/demo2", srv, QDBusConnection::ExportAdaptors);
    return 0;
}
