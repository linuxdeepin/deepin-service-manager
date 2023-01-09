#include "servicemanager.h"
#include "service/serviceqtdbus.h"
#include "service/servicesdbus.h"

#include <QDebug>
#include <QDBusMessage>
#include <QThread>
#include <QDir>

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

// TODO
static const QStringList CompatiblePluginApiList {
    "1.0",
};

ServiceManager::ServiceManager(QObject *parent) : DDEQDBusService(parent)
  , m_sessionType(SessionType::Unknown)
{
}

ServiceManager::~ServiceManager()
{
    foreach(auto srv, m_pluginMap.values()) {
        // TODO
    }
    m_pluginMap.clear();
}

ServiceBase *ServiceManager::createService(SessionType sessionType, SDKType sdkType, QString configPath)
{
    ServiceBase *srv = nullptr;
    if (sdkType == SDKType::QT) {
        srv = new ServiceQtDBus();
    } else if (sdkType == SDKType::SD) {
        srv = new ServiceSDBus();
    } else {
        qInfo() << __FUNCTION__ << "error config:" << configPath;
    }
    if (srv) {
        srv->Init(sessionType, configPath);
        qInfo() << "[ServiceManager]Init plugin finish." << srv->libPath();
    }

    return srv;
}

void ServiceManager::serverInit(SessionType type)
{
    m_sessionType = type;

    if (m_sessionType == SessionType::Session) {
        DDEQDBusService::InitPolicy(QDBusConnection::SessionBus, QString(DEEPIN_SERVICE_MANAGER_DIR)+ServiceManagerDBusPolicyFile);
        loadPlugins(m_sessionType, SDKType::QT, QString(DEEPIN_SERVICE_MANAGER_DIR)+"user/plugin-qt-service");
        loadPlugins(m_sessionType, SDKType::SD, QString(DEEPIN_SERVICE_MANAGER_DIR)+"user/plugin-sd-service");
    } else if(m_sessionType == SessionType::System) {
        DDEQDBusService::InitPolicy(QDBusConnection::SystemBus, QString(DEEPIN_SERVICE_MANAGER_DIR)+ServiceManagerDBusPolicyFile);
        loadPlugins(m_sessionType, SDKType::QT, QString(DEEPIN_SERVICE_MANAGER_DIR)+"system/plugin-qt-service");
        loadPlugins(m_sessionType, SDKType::SD, QString(DEEPIN_SERVICE_MANAGER_DIR)+"system/plugin-sd-service");
    } else {
        qWarning() << "init plugin failed:unknown type";
    }
}

bool ServiceManager::loadPlugins(SessionType sessionType, SDKType sdkType, QString path)
{
    qInfo() << "Init Plugins:" << path;
    QFileInfoList list = QDir(path).entryInfoList();
    for (auto file : list) {
        if (!file.isFile() || (file.suffix().compare("json", Qt::CaseInsensitive) != 0)) {
            continue;
        }

        ServiceBase *srv = createService(sessionType, sdkType, file.absoluteFilePath());
        if (srv != nullptr) {
            addPlugin(srv); // TODO:插件列表和 sdbus和qtbus等统一设计
        }
    }
    return true;
}

QString ServiceManager::getPlugins()
{
    // TODO
    return "";
}

bool ServiceManager::addPlugin(ServiceBase *obj)
{
    if (obj->libPath().isEmpty()) {
        return false;
    }
    m_pluginMap[obj->libPath()] = obj->paths();
    return true;
}
//m_pluginMap
QString ServiceManager::Version()
{
    return "1.0";
}

QString ServiceManager::Plugins()
{
    QJsonArray pluginArray;
    for (auto iter = m_pluginMap.begin(); iter != m_pluginMap.end(); ++iter) {
        QJsonObject pluginObject;
        pluginObject.insert("plugin", iter.key());
        QJsonArray pathArray;
        for (auto path : iter.value()) {
            pathArray.append(QJsonValue(path));
        }
        pluginObject.insert("path", pathArray);
        pluginArray.append(pluginObject);
    }
    QJsonDocument jsonDoc;
    jsonDoc.setArray(pluginArray);
    return QString(jsonDoc.toJson());
}
