#include "servicemanager.h"

#include <QCoreApplication>
#include <QDBusError>
#include <QDBusInterface>
#include <QDebug>
#include <QDir>
#include <QFileInfoList>
#include <QProcess>

#include "groupmanager.h"
#include "policy/policy.h"
#include "servicemanagerprivate.h"
#include "servicemanagerpublic.h"
#include "utils.h"

ServiceManager::ServiceManager(QObject *parent)
    : QObject(parent)
    , m_publicService(new ServiceManagerPublic(this))
    , m_privateService(new ServiceManagerPrivate(this))
    , m_connection(QDBusConnection::sessionBus())
{
    initConnection();
}

ServiceManager::~ServiceManager() {}

void ServiceManager::init(const QDBusConnection::BusType &type)
{
    m_publicService->init(type);
    m_privateService->init(type);

    if (type == QDBusConnection::SystemBus)
        m_connection = QDBusConnection::systemBus();

    if (!m_connection.registerService(ServiceManagerName)) {
        qWarning() << "[ServiceManager]failed to register dbus service:"
                   << m_connection.lastError().message();
    }
    if (!m_connection.registerObject(
            ServiceManagerPath,
            m_publicService,
            QDBusConnection::ExportScriptableContents |
                QDBusConnection::ExportAllProperties)) {
        qWarning() << "[ServiceManager]failed to register dbus object: "
                   << m_connection.lastError().message();
    }
    if (!m_connection.registerObject(ServiceManagerPrivatePath,
                                     m_privateService,
                                     QDBusConnection::ExportAllSlots |
                                         QDBusConnection::ExportAllSignals)) {
        qWarning() << "[ServiceManager]failed to register dbus object: "
                   << m_connection.lastError().message();
    }

    initGroup(type);
}

void ServiceManager::initGroup(const QDBusConnection::BusType &type)
{
    QStringList groups;
    const QString &qtConfigPath =
        QString("%1/%2/qt-service").arg(SERVICE_CONFIG_DIR).arg(typeMap[type]);
    const QString &sdConfigPath =
        QString("%1/%2/sd-service").arg(SERVICE_CONFIG_DIR).arg(typeMap[type]);

    QFileInfoList list = QDir(qtConfigPath).entryInfoList();
    list.append(QDir(sdConfigPath).entryInfoList());
    for (auto &&file : list) {
        if (!file.isFile() ||
            (file.suffix().compare("json", Qt::CaseInsensitive) != 0)) {
            continue;
        }
        Policy *policy = new Policy(this);
        policy->ParseConfig(file.absoluteFilePath());
        if (!groups.contains(policy->m_group) && !policy->m_group.isEmpty()) {
            groups.append(policy->m_group);
        }
        policy->deleteLater();
    }
    qDebug() << "[ServiceManager]groups: " << groups;

    for (auto &&group : groups) {
        QProcess *process = new QProcess(this);
        process->start(qApp->applicationFilePath(),
                       {"-c", typeMap[type] + '.' + group});
    }
}

void ServiceManager::initConnection()
{
    connect(m_privateService,
            &ServiceManagerPrivate::requestRegisterGroup,
            this,
            &ServiceManager::onRegisterGroup);
}

void ServiceManager::onRegisterGroup(const QString &groupName,
                                     const QString &serviceName)
{
    qDebug() << "[ServiceManager]on register group:" << groupName
             << serviceName;
    if (m_groups.contains(groupName))
        return;
    GroupManager *groupManager = new GroupManager(this);
    GroupData data;
    data.ServiceName = serviceName;
    data.GroupManager = groupManager;
    m_groups[groupName] = data;

    m_publicService->addGroup(groupName);
    const QString &groupPath = "/group/" + groupName;

    // register group path
    if (!m_connection.registerObject(
            groupPath,
            groupManager,
            QDBusConnection::ExportScriptableContents |
                QDBusConnection::ExportAllProperties)) {
        qWarning() << "[ServiceManager]failed to register dbus object: "
                   << m_connection.lastError().message();
    }
    // connect plugin manager, call method
    m_connection.connect(serviceName,
                         PluginManagerPath,
                         PluginManagerInterface,
                         "PluginAdded",
                         groupManager,
                         SLOT(addPlugin(const QString &)));
}
