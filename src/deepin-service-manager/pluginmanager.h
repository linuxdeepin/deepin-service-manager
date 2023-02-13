#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include "service/servicebase.h"
#include "utils.h"

#include <qlist.h>

#include <QMap>
#include <QObject>

typedef QMap<QString, ServiceBase *> PluginMap;

class PluginManager : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", PluginManagerInterface);
    Q_PROPERTY(QStringList Plugins READ plugins);

public:
    explicit PluginManager(const QDBusConnection &connection, QObject *parent = nullptr);
    ~PluginManager();

    void init(const QDBusConnection::BusType &type, const QString &group);

signals:
    Q_SCRIPTABLE void PluginAdded(const QString &plugin);

private:
    ServiceBase *createService(const QDBusConnection::BusType &sessionType, Policy *policy);
    bool loadPlugins(const QDBusConnection::BusType &sessionType, const QString &path);
    void addPlugin(ServiceBase *obj);
    QList<Policy *> sortPolicy(QList<Policy *> policys);

    QStringList plugins() const;

private:
    PluginMap m_pluginMap;
    QDBusConnection m_connection;
    QString m_group;
};

#endif // PLUGINMANAGER_H
