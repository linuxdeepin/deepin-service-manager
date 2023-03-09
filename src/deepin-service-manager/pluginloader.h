// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef PLUGINLOADER
#define PLUGINLOADER

#include <QDBusConnection>
#include <QMap>

class ServiceBase;
class Policy;
typedef QMap<QString, ServiceBase *> PluginMap;

class PluginLoader : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList Plugins READ plugins);

public:
    explicit PluginLoader(QObject *parent = nullptr);
    ~PluginLoader();

    void init(const QDBusConnection::BusType &type, const QString &group);

signals:
    void PluginAdded(const QString &plugin);

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

#endif // PLUGINLOADER
