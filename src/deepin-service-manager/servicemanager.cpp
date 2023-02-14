#include "servicemanager.h"

#include "groupmanager.h"
#include "policy/policy.h"
#include "servicemanagerprivate.h"
#include "servicemanagerpublic.h"
#include "utils.h"

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

    initGroup(type);
}

void ServiceManager::initGroup(const QDBusConnection::BusType &type)
{
    m_busType = type;
    QDBusConnection connection = QDBusConnection::connectToBus(type, ServiceManagerName);

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
    for (auto &&group : groups) {
        if (!QString("Debug").compare(BUILD_TYPE)) {
            QProcess *process = new QProcess(this);
            process->start(qApp->applicationFilePath(), { "-c", typeMap[type] + '.' + group });

        } else {
            QDBusInterface remote("org.freedesktop.systemd1",
                                  "/org/freedesktop/systemd1",
                                  "org.freedesktop.systemd1.Manager",
                                  connection);
            remote.call(
                    "StartUnit",
                    QString("deepin-service-manager@%1.%2.service").arg(typeMap[type]).arg(group),
                    "fail");
        }
    }
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
    if (m_groups.contains(groupName))
        return;
    GroupManager *groupManager = new GroupManager(this);
    GroupData data;
    data.ServiceName = serviceName;
    data.GroupManager = groupManager;
    m_groups[groupName] = data;

    m_publicService->addGroup(groupName);
    const QString &groupPath = "/group/" + groupName;

    QDBusConnection connection = QDBusConnection::connectToBus(m_busType, ServiceManagerName);
    // register group path
    if (!connection.registerObject(groupPath,
                                   groupManager,
                                   QDBusConnection::ExportScriptableContents
                                           | QDBusConnection::ExportAllProperties)) {
        qWarning() << "[ServiceManager]failed to register dbus object: "
                   << connection.lastError().message();
    }
    // connect plugin manager, call method
    connection.connect(serviceName,
                       PluginManagerPath,
                       PluginManagerInterface,
                       "PluginAdded",
                       groupManager,
                       SLOT(addPlugin(const QString &)));
}
