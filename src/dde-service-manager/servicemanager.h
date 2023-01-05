#ifndef SERVICEMANAGER_H
#define SERVICEMANAGER_H

#include "service/servicebase.h"
#include "libddeqdbusservice/ddeqdbusservice.h"

#include <QObject>
#include <QGlobalStatic>
#include <QMap>


typedef QMap<QString, ServiceBase*> ServiceObjectMap;  // TODO
typedef QMap<QString, QStringList> PluginMap;

const QString ServiceManagerDBusName = "org.deepin.services.manager";
const QString ServiceManagerDBusPath = "/org/deepin/services/manager";
const QString ServiceManagerDBusIntterface = "org.deepin.services.manager"; // TODO
const QString ServiceManagerDBusPolicyFile = "other/dde-qt-service/manager.json";

class ServiceManager : public DDEQDBusService
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.deepin.services.manager")
public:
    explicit ServiceManager(QObject *parent = nullptr);
    ~ServiceManager();

    ServiceBase *createService(SessionType sessionType, SDKType sdkType, QString configPath);

    void serverInit(SessionType type);

    bool loadPlugins(SessionType sessionType, SDKType sdkType, QString path);

    QString getPlugins();

    bool addPlugin(ServiceBase *obj);

public slots:
    QString Version();
    QString Plugins();

private:
    PluginMap m_pluginMap;
    SessionType m_sessionType;
};

#endif // SERVICEMANAGER_H
