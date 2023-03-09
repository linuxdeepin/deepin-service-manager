// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "graph.h"
#include "pluginloader.h"
#include "policy/policy.h"
#include "service/serviceqtdbus.h"
#include "service/servicesdbus.h"
#include "utils.h"

#include <QDebug>
#include <QDir>
#include <QEventLoop>
#include <QTimer>

PluginLoader::PluginLoader(QObject *parent)
    : QObject(parent)
{
}

PluginLoader::~PluginLoader()
{
    foreach (auto srv, m_pluginMap.values()) {
        delete srv;
        srv = nullptr;
    }
    m_pluginMap.clear();
}

ServiceBase *PluginLoader::createService(const QDBusConnection::BusType &sessionType,
                                         Policy *policy)
{
    ServiceBase *srv = nullptr;
    if (policy->sdkType == SDKType::QT)
        srv = new ServiceQtDBus();
    if (policy->sdkType == SDKType::SD)
        srv = new ServiceSDBus();
    if (srv) {
        srv->init(sessionType, policy);
        qInfo() << "[PluginLoader]init plugin finish:" << srv->policy->libPath;
    }

    return srv;
}

void PluginLoader::init(const QDBusConnection::BusType &type, const QString &group)
{
    m_group = group;
    // load plugin
    loadPlugins(type);
}

bool PluginLoader::loadPlugins(const QDBusConnection::BusType &type)
{
    const QString &path = QString("%1/%2/").arg(SERVICE_CONFIG_DIR).arg(typeMap[type]);
    qInfo() << "[PluginLoader]init plugins:" << path;
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
                addPlugin(srv);
            }
            loop->quit();
        });
        loop->exec();
    }
    qDebug() << "[PluginLoader]added plugins: " << m_pluginMap.keys();
    return true;
}

void PluginLoader::addPlugin(ServiceBase *obj)
{
    m_pluginMap[obj->policy->name] = obj;
    emit PluginAdded(obj->policy->name);
}

QList<Policy *> PluginLoader::sortPolicy(QList<Policy *> policys)
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
                qWarning() << QString("[PluginLoader]service: %1 cannot found "
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
    qDebug() << "[PluginLoader]sort result name:";
    for (auto policy : result) {
        qDebug() << "  " << policy->name;
    }
    return result;
}

QStringList PluginLoader::plugins() const
{
    return m_pluginMap.keys();
}
