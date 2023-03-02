#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include "utils.h"

#include <QMap>
#include <QObject>

class PluginManager : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", PluginManagerInterface);
    Q_PROPERTY(QStringList Plugins READ plugins);

public:
    explicit PluginManager(QObject *parent = nullptr);

    void init(const QDBusConnection::BusType &type, const QString &group);

signals:
    Q_SCRIPTABLE void PluginAdded(const QString &plugin);

private:
    QStringList plugins() const;

private:
    QStringList m_plugins;
    QString m_group;
};

#endif // PLUGINMANAGER_H
