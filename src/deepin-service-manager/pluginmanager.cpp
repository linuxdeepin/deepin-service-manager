// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "pluginmanager.h"

#include "pluginloader.h"
#include "utils.h"

#include <QDBusInterface>
#include <QDebug>

PluginManager::PluginManager(QObject *parent)
    : QObject(parent)
{
}

void PluginManager::init(const QDBusConnection::BusType &type, const QString &group)
{
    m_group = group;
    // Register service and call pluginmanager
    QDBusConnection connection = type == QDBusConnection::SessionBus ? QDBusConnection::sessionBus()
                                                                     : QDBusConnection::systemBus();
    if (!connection.registerObject(PluginManagerPath,
                                   this,
                                   QDBusConnection::ExportScriptableContents
                                           | QDBusConnection::ExportAllProperties)) {
        qWarning() << "[PluginManager]failed to register dbus object: "
                   << connection.lastError().message();
    }

    QDBusInterface remote(ServiceManagerName,
                          ServiceManagerPrivatePath,
                          ServiceManagerInterface,
                          connection);
    remote.call("RegisterGroup", m_group, connection.baseService());

    PluginLoader *loader = new PluginLoader(this);
    connect(loader, &PluginLoader::PluginAdded, this, [this](const QString &name) {
        m_plugins.append(name);
        Q_EMIT PluginAdded(name);
    });
    loader->init(type, group);
}

QStringList PluginManager::plugins() const
{
    return m_plugins;
}
