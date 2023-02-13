#ifndef MANAGER_UTILS_H
#define MANAGER_UTILS_H

#include <QDBusConnection>

#define ServiceManagerInterface "org.deepin.service.manager1"
#define PluginManagerInterface "org.deepin.service.manager.Plugin1"

static const QString &ServiceManagerName = QStringLiteral("org.deepin.service.manager");
static const QString &ServiceManagerPath = QStringLiteral("/manager");
static const QString &ServiceManagerPrivatePath = QStringLiteral("/manager/private");

static const QString &PluginManagerPath = QStringLiteral("/manager/Plugin");

static const QMap<QDBusConnection::BusType, QString> typeMap{
    { QDBusConnection::SystemBus, "system" }, { QDBusConnection::SessionBus, "user" }
};
#endif // MANAGER_UTILS_H
