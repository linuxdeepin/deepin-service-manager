#include "pluginmanager.h"

#include "graph.h"
#include "policy/policy.h"
#include "service/serviceqtdbus.h"
#include "service/servicesdbus.h"
#include "utils.h"

#include <QDBusInterface>
#include <QDBusMessage>
#include <QDebug>
#include <QDir>
#include <QEventLoop>
#include <QThread>
#include <QTimer>

PluginManager::PluginManager(QObject *parent)
    : QObject(parent)
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

ServiceBase *PluginManager::createService(const QDBusConnection::BusType &sessionType,
                                          Policy *policy)
{
    ServiceBase *srv = nullptr;
    if (policy->sdkType == SDKType::QT)
        srv = new ServiceQtDBus();
    if (policy->sdkType == SDKType::SD)
        srv = new ServiceSDBus();
    if (srv) {
        srv->init(sessionType, policy);
        qInfo() << "[PluginManager]init plugin finish:" << srv->policy->libPath;
    }

    return srv;
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

    // load plugin
    loadPlugins(type, QString("%1/%2/").arg(SERVICE_CONFIG_DIR).arg(typeMap[type]));
}

bool PluginManager::loadPlugins(const QDBusConnection::BusType &type, const QString &path)
{
    qInfo() << "[PluginManager]init plugins:" << path;
    QList<Policy *> policys;
    QFileInfoList list = QDir(path).entryInfoList();
    for (auto file : list) {
        if (!file.isFile() || (file.suffix().compare("json", Qt::CaseInsensitive) != 0)) {
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
    // sort policy
    const QList<Policy *> &sortedPolicys = sortPolicy(policys);
    for (auto policy : sortedPolicys) {
        // start delay
        const int delay = policy->startDelay * 1000;
        QEventLoop *loop = new QEventLoop(this);
        QTimer::singleShot(delay, this, [this, type, policy, loop] {
            QDBusConnection connection = type == QDBusConnection::SessionBus
                    ? QDBusConnection::sessionBus()
                    : QDBusConnection::systemBus();
            ServiceBase *srv = createService(type, policy);
            if (srv == nullptr) {
                loop->quit();
                return;
            }
            if (!policy->libPath.isEmpty()) {
                QDBusInterface remote(ServiceManagerName,
                                      ServiceManagerPrivatePath,
                                      ServiceManagerInterface,
                                      connection);
                remote.call("RegisterGroup", m_group, connection.baseService());
                addPlugin(srv);
            }
            loop->quit();
        });
        loop->exec();
    }
    qDebug() << "[PluginManager]added plugins: " << m_pluginMap.keys();
    return true;
}

void PluginManager::addPlugin(ServiceBase *obj)
{
    m_pluginMap[obj->policy->name] = obj;
    emit PluginAdded(obj->policy->name);
}

QList<Policy *> PluginManager::sortPolicy(QList<Policy *> policys)
{
    // 使用拓扑排序，先确定依赖关系
    auto containsDependency = [policys](const QString &name) -> Policy * {
        for (auto &&policy : policys) {
            if (policy->name == name)
                return policy;
        }
        return nullptr;
    };
    QList<QPair<Policy *, Policy *>> edges;
    for (auto &&policy : policys) {
        for (auto &&dependency : policy->dependencies) {
            if (Policy *dependencyPolicy = containsDependency(dependency)) {
                edges.append(QPair<Policy *, Policy *>{ dependencyPolicy, policy });
            } else {
                qWarning() << QString("[PluginManager]service: %1 cannot found "
                                      "dependency: %2!")
                                      .arg(policy->name)
                                      .arg(dependency);
            }
        }
    }
    // 拓扑排序
    QScopedPointer<Graph<Policy *>> graph(new Graph<Policy *>(policys, edges));
    QList<Policy *> result;
    graph->topologicalSort(result);
    qDebug() << "[PluginManager]sort result name:";
    for (auto policy : result) {
        qDebug() << "  " << policy->name;
    }
    return result;
}

QStringList PluginManager::plugins() const
{
    return m_pluginMap.keys();
}
