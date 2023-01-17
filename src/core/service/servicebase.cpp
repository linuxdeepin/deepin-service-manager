#include "servicebase.h"

#include <QDBusConnection>
#include <QDebug>
#include <QDBusMessage>
#include <QThread>


ServiceBase::ServiceBase(QObject *parent) : QObject(parent)
    , m_isRegister(false)
    , m_policy(nullptr)
{

}

ServiceBase::~ServiceBase()
{
}

//bool ServiceBase::InitConfig(QString path)
//{
//    m_pluginConfig = new PluginConfig();
//    m_pluginConfig->loadFile(path);
//}

void ServiceBase::Init(const QDBusConnection::BusType busType, const QString &configPath)
{
    m_sessionType = busType;
//    InitConfig(configPath);
    m_policy = new Policy(this);
    m_policy->ParseConfig(configPath);
    // m_policy->Print(); // TODO

    qInfo() << "[Service]Base init." << name();
    InitService();
}

//void ServiceBase::InitByData(SessionType sessionType, const QMap<QString, QVariant> &data)
//{
//    m_sessionType = sessionType;
//    m_pluginConfig = new PluginConfig();
//    m_pluginConfig->loadData(data);

//    InitService();
//}

void ServiceBase::InitService()
{

}

bool ServiceBase::IsRegister()
{
    return m_isRegister;
}

bool ServiceBase::CheckMethodPermission(QString process, QString path, QString interface, QString method)
{
    if (m_policy == nullptr) {
        return false;
    }
    return m_policy->CheckMethodPermission(process, path, interface, method);
}

bool ServiceBase::CheckPropertyPermission(QString process, QString path, QString interface, QString property)
{
    if (m_policy == nullptr) {
        return false;
    }
    return m_policy->CheckPropertyPermission(process, path, interface, property);
}

bool ServiceBase::CheckPathHide(QString path)
{
    if (m_policy == nullptr) {
        return false;
    }
    return m_policy->CheckPathHide(path);
}

bool ServiceBase::Register()
{
    qInfo() << "[Service]Base register";
    m_isRegister = true;
    return true;
}

Qt::HANDLE ServiceBase::threadID()
{
    return QThread::currentThreadId();
}

QStringList ServiceBase::paths()
{
//    if (m_pluginConfig) {
//        return m_pluginConfig->m_dbusPath;
//    }
    if (m_policy) {
        return m_policy->m_mapSubPath.keys();
    }
    return QStringList();
}

bool ServiceBase::allowSubPath(QString path)
{
    if (m_policy) {
        auto iter = m_policy->m_mapSubPath.find(path);
        if (iter != m_policy->m_mapSubPath.end()) {
            return iter.value();
        }
    }
    return false;
}

QString ServiceBase::name()
{
//    if (m_pluginConfig) {
//        return m_pluginConfig->m_dbusName;
//    }
    if (m_policy) {
        return m_policy->m_name;
    }
    return "";
}

QString ServiceBase::group() const
{
    if (m_policy)
        return m_policy->m_group;
    return "";
}

//QString ServiceBase::interface()
//{
//    if (m_pluginConfig) {
//        return m_pluginConfig->m_dbusInterface;
//    }
//    return "";
//}

QString ServiceBase::libPath()
{
//    if (m_pluginConfig) {
//        return m_pluginConfig->m_libPath;
//    }
    if (m_policy) {
        return m_policy->m_libPath;
    }
    return "";
}

//QString ServiceBase::policyPath()
//{
//    if (m_pluginConfig) {
//        return m_pluginConfig->m_policyPath;
//    }
//    return "";
//}

bool ServiceBase::isResident()
{
//    if (m_pluginConfig && m_pluginConfig->m_startType == "Resident") {
//        return true;
//    }
    if (m_policy && m_policy->m_policyStartType == "Resident") {
        return true;
    }
    return false;
}

void ServiceBase::test()
{

}

