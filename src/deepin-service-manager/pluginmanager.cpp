#include "pluginmanager.h"

#include <QDBusInterface>
#include <QDBusMessage>
#include <QDebug>
#include <QDir>
#include <QThread>

#include "service/serviceqtdbus.h"
#include "service/servicesdbus.h"
#include "utils.h"

// TODO
static const QStringList CompatiblePluginApiList{
    "1.0",
};

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
    const QDBusConnection::BusType &sessionType,
    const SDKType &sdkType,
    const QString &configPath)
{
    ServiceBase *srv = nullptr;
    if (sdkType == SDKType::QT) {
        srv = new ServiceQtDBus();
    } else if (sdkType == SDKType::SD) {
        srv = new ServiceSDBus();
    } else {
        qWarning() << "[PluginManager]error config:" << configPath;
    }
    if (srv) {
        srv->Init(sessionType, configPath);
        qInfo() << "[PluginManager]Init plugin finish." << srv->libPath();
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
    loadPlugins(
        type,
        SDKType::QT,
        QString("%1/%2/qt-service").arg(SERVICE_CONFIG_DIR).arg(typeMap[type]));
    loadPlugins(
        type,
        SDKType::SD,
        QString("%1/%2/sd-service").arg(SERVICE_CONFIG_DIR).arg(typeMap[type]));
}

bool PluginManager::loadPlugins(const QDBusConnection::BusType &sessionType,
                                const SDKType &sdkType,
                                const QString &path)
{
    qInfo() << "[PluginManager]Init Plugins:" << path;
    QFileInfoList list = QDir(path).entryInfoList();
    for (auto file : list) {
        if (!file.isFile() ||
            (file.suffix().compare("json", Qt::CaseInsensitive) != 0)) {
            continue;
        }

        ServiceBase *srv =
            createService(sessionType, sdkType, file.absoluteFilePath());
        if (srv == nullptr)
            continue;
        if (srv->group() != m_group)
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
    if (obj->libPath().isEmpty()) {
        return false;
    }
    m_pluginMap[obj->name()] = obj;
    emit PluginAdded(obj->name());
    return true;
}

QStringList PluginManager::plugins() const
{
    return m_pluginMap.keys();
}
