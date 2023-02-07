#include "pluginmanager.h"

#include <QDBusInterface>
#include <QDBusMessage>
#include <QDebug>
#include <QDir>
#include <QThread>

#include "policy/policy.h"
#include "service/serviceqtdbus.h"
#include "service/servicesdbus.h"
#include "utils.h"

PluginManager::PluginManager(QObject *parent)
    : QObject(parent)
    , m_connection(QDBusConnection::sessionBus())
{
}

PluginManager::~PluginManager()
{
    foreach (auto srv, m_pluginMap.values()) {
        delete srv;
        srv = nullptr;
    }
    m_pluginMap.clear();
}

ServiceBase *PluginManager::createService(
    const QDBusConnection::BusType &sessionType, Policy *policy)
{
    ServiceBase *srv = nullptr;
    if (policy->sdkType == SDKType::QT)
        srv = new ServiceQtDBus();
    if (policy->sdkType == SDKType::SD)
        srv = new ServiceSDBus();
    if (srv) {
        srv->init(sessionType, policy);
        qInfo() << "[PluginManager]Init plugin finish." << srv->policy->libPath;
    }

    return srv;
}

void PluginManager::init(const QDBusConnection::BusType &type,
                         const QString &group)
{
    m_group = group;
    // Register service and call pluginmanager
    if (type == QDBusConnection::SystemBus)
        m_connection = QDBusConnection::systemBus();
    if (!m_connection.registerObject(
            PluginManagerPath,
            this,
            QDBusConnection::ExportScriptableContents |
                QDBusConnection::ExportAllProperties)) {
        qWarning() << "[PluginManager]failed to register dbus object: "
                   << m_connection.lastError().message();
    }

    // load plugin
    loadPlugins(type,
                QString("%1/%2/").arg(SERVICE_CONFIG_DIR).arg(typeMap[type]));
}

bool PluginManager::loadPlugins(const QDBusConnection::BusType &sessionType,
                                const QString &path)
{
    qInfo() << "[PluginManager]Init Plugins:" << path;
    QList<Policy *> policys;
    QFileInfoList list = QDir(path).entryInfoList();
    for (auto file : list) {
        if (!file.isFile() ||
            (file.suffix().compare("json", Qt::CaseInsensitive) != 0)) {
            continue;
        }
        Policy *policy = new Policy(this);
        policy->parseConfig(file.absoluteFilePath());
        if (policy->group != m_group) {
            policy->deleteLater();
            continue;
        }
        policys.append(policy);
    }
    for (auto policy : policys) {
        ServiceBase *srv = createService(sessionType, policy);
        if (srv == nullptr)
            continue;
        QDBusInterface remote(ServiceManagerName,
                              ServiceManagerPrivatePath,
                              ServiceManagerInterface,
                              m_connection);
        remote.call("RegisterGroup", m_group, m_connection.baseService());
        addPlugin(srv);  // TODO:插件列表和 sdbus和qtbus等统一设计
    }
    qDebug() << "[PluginManager]plugin map: " << m_pluginMap;
    return true;
}

bool PluginManager::addPlugin(ServiceBase *obj)
{
    if (obj->policy->libPath.isEmpty()) {
        return false;
    }
    m_pluginMap[obj->policy->name] = obj;
    emit PluginAdded(obj->policy->name);
    return true;
}

QStringList PluginManager::plugins() const
{
    return m_pluginMap.keys();
}
