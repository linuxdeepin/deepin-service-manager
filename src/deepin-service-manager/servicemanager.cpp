#include "servicemanager.h"

#include "groupmanager.h"
#include "pluginloader.h"
#include "policy/policy.h"
#include "servicemanagerprivate.h"
#include "servicemanagerpublic.h"

#include <QCoreApplication>
#include <QDBusError>
#include <QDBusInterface>
#include <QDebug>
#include <QDir>
#include <QFileInfoList>
#include <QProcess>

static const QStringList GroupSiral{ "core", "dde", "app" };

ServiceManager::ServiceManager(QObject *parent)
    : QObject(parent)
    , m_publicService(new ServiceManagerPublic(this))
    , m_privateService(new ServiceManagerPrivate(this))
{
    initConnection();
}

ServiceManager::~ServiceManager() { }

void ServiceManager::init(const QDBusConnection::BusType &type)
{
    m_publicService->init(type);
    m_privateService->init(type);

    QDBusConnection connection = m_publicService->qDbusConnection();

    if (!connection.registerService(ServiceManagerName)) {
        qWarning() << "[ServiceManager]failed to register dbus service:"
                   << connection.lastError().message();
    }
    if (!connection.registerObject(ServiceManagerPath,
                                   m_publicService,
                                   QDBusConnection::ExportScriptableContents
                                           | QDBusConnection::ExportAllProperties)) {
        qWarning() << "[ServiceManager]failed to register dbus object: "
                   << connection.lastError().message();
    }
    if (!connection.registerObject(ServiceManagerPrivatePath,
                                   m_privateService,
                                   QDBusConnection::ExportAllSlots
                                           | QDBusConnection::ExportAllSignals)) {
        qWarning() << "[ServiceManager]failed to register dbus object: "
                   << connection.lastError().message();
    }

    initGroup(type);
}

void ServiceManager::initGroup(const QDBusConnection::BusType &type)
{
    m_busType = type;
    QStringList groups;
    const QString &configPath = QString("%1/%2/").arg(SERVICE_CONFIG_DIR).arg(typeMap[type]);

    QFileInfoList list = QDir(configPath).entryInfoList();
    Policy *policy = new Policy(this);
    for (auto &&file : list) {
        if (!file.isFile() || (file.suffix().compare("json", Qt::CaseInsensitive) != 0)) {
            continue;
        }
        policy->parseConfig(file.absoluteFilePath());
        if (!groups.contains(policy->group) && !policy->group.isEmpty()) {
            groups.append(policy->group);
        }
    }
    policy->deleteLater();
    // sort groups
    std::sort(groups.begin(),
              groups.end(),
              [](const QString &group1, const QString &group2) -> bool {
                  if (!GroupSiral.contains(group1))
                      return false;
                  return GroupSiral.indexOf(group1) < GroupSiral.indexOf(group2);
              });
    qDebug() << "[ServiceManager]groups: " << groups;
    // launch core group
    const QString &core = "core";
    if (groups.contains(core)) {
        auto groupManager = addGroup(core);
        PluginLoader *plugin = new PluginLoader(this);
        connect(plugin, &PluginLoader::PluginAdded, this, [groupManager](const QString &name) {
            groupManager->addPlugin(name);
        });
        plugin->init(type, core);
        groups.removeOne(core);
    }
    qDebug() << "groups:" << groups;
    for (auto &&group : groups) {
        QDBusInterface remote("org.freedesktop.systemd1",
                              "/org/freedesktop/systemd1",
                              "org.freedesktop.systemd1.Manager",
                              m_publicService->qDbusConnection());
        remote.call("StartUnit", QString("deepin-service-plugin@%1.service").arg(group), "fail");
    }
}

GroupManager *ServiceManager::addGroup(const QString &group)
{
    if (m_publicService->groups().contains(group))
        return nullptr;
    GroupManager *groupManager = new GroupManager(this);

    m_publicService->addGroup(group);
    const QString &groupPath = "/group/" + group;

    QDBusConnection connection = m_publicService->qDbusConnection();
    // register group path
    if (!connection.registerObject(groupPath,
                                   groupManager,
                                   QDBusConnection::ExportScriptableContents
                                           | QDBusConnection::ExportAllProperties)) {
        qWarning() << "[ServiceManager]failed to register dbus object: "
                   << connection.lastError().message();
    }
    return groupManager;
}

void ServiceManager::initConnection()
{
    connect(m_privateService,
            &ServiceManagerPrivate::requestRegisterGroup,
            this,
            &ServiceManager::onRegisterGroup);
}

void ServiceManager::onRegisterGroup(const QString &groupName, const QString &serviceName)
{
    qDebug() << "[ServiceManager]on register group:" << groupName << serviceName;
    auto groupManager = addGroup(groupName);
    if (groupManager) {
        // connect plugin manager, call method
        QDBusConnection connection = m_publicService->qDbusConnection();
        connection.connect(serviceName,
                           PluginManagerPath,
                           PluginManagerInterface,
                           "PluginAdded",
                           groupManager,
                           SLOT(addPlugin(const QString &)));
    }
}
