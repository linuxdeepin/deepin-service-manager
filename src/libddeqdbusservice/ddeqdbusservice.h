#ifndef DDEQDBUSSERVICE_H
#define DDEQDBUSSERVICE_H

#include <QObject>
#include <QDBusConnection>

#if defined(EXPORT_LIBRARY)
#  define EXPORT_PLUGIN_LIBRARY Q_DECL_EXPORT // TODO
#else
#  define EXPORT_PLUGIN_LIBRARY Q_DECL_IMPORT
#endif

class DDEQDBusServicePrivate;
class EXPORT_PLUGIN_LIBRARY DDEQDBusService : public QObject
{
    Q_OBJECT
    DDEQDBusServicePrivate *d_ptr;
    Q_DECLARE_PRIVATE(DDEQDBusService)

public:
    explicit DDEQDBusService(QObject *parent = nullptr);
    void InitPolicy(QDBusConnection::BusType busType, QString policyFile);
};

#endif // DDEQDBUSSERVICE_H
