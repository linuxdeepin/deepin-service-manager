#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include "../utils.h"

#include <QMap>
#include <QObject>

class ServiceBase;
class Policy;
typedef QMap<QString, ServiceBase *> PluginMap;

class PluginManager : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", PluginManagerInterface);
    Q_PROPERTY(QStringList Plugins READ plugins);

public:
    explicit PluginManager(QObject *parent = nullptr);
    ~PluginManager();

    void init(const QDBusConnection::BusType &type, const QString &group);

signals:
    Q_SCRIPTABLE void PluginAdded(const QString &plugin);

private:
    ServiceBase *createService(const QDBusConnection::BusType &sessionType, Policy *policy);
    bool loadPlugins(const QDBusConnection::BusType &sessionType);
    void addPlugin(ServiceBase *obj);
    QList<Policy *> sortPolicy(QList<Policy *> policys);

    QStringList plugins() const;

private:
    PluginMap m_pluginMap;
    QString m_group;
};

#endif // PLUGINMANAGER_H
