#include "servicebase.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDebug>
#include <QThread>

#include "policy/policy.h"

ServiceBase::ServiceBase(QObject *parent)
    : QObject(parent)
    , m_isRegister(false)
    , m_policy(nullptr)
{
}

ServiceBase::~ServiceBase() {}

void ServiceBase::Init(const QDBusConnection::BusType &busType,
                       const QString &configPath)
{
    m_sessionType = busType;
    m_policy = new Policy(this);
    m_policy->ParseConfig(configPath);
    // m_policy->Print(); // TODO

    qInfo() << "[ServiceBase]Base init." << name();
    InitService();
}

void ServiceBase::InitService() {}

bool ServiceBase::IsRegister() const
{
    return m_isRegister;
}

bool ServiceBase::CheckMethodPermission(const QString &process,
                                        const QString &path,
                                        const QString &interface,
                                        const QString &method) const
{
    if (m_policy == nullptr) {
        return false;
    }
    return m_policy->CheckMethodPermission(process, path, interface, method);
}

bool ServiceBase::CheckPropertyPermission(const QString &process,
                                          const QString &path,
                                          const QString &interface,
                                          const QString &property) const
{
    if (m_policy == nullptr) {
        return false;
    }
    return m_policy->CheckPropertyPermission(
        process, path, interface, property);
}

bool ServiceBase::CheckPathHide(const QString &path) const
{
    if (m_policy == nullptr) {
        return false;
    }
    return m_policy->CheckPathHide(path);
}

bool ServiceBase::Register()
{
    qInfo() << "[ServiceBase]Base register";
    m_isRegister = true;
    return true;
}

Qt::HANDLE ServiceBase::threadID()
{
    return QThread::currentThreadId();
}

QStringList ServiceBase::paths() const
{
    if (m_policy) {
        return m_policy->m_mapSubPath.keys();
    }
    return QStringList();
}

bool ServiceBase::allowSubPath(const QString &path) const
{
    if (m_policy) {
        auto iter = m_policy->m_mapSubPath.find(path);
        if (iter != m_policy->m_mapSubPath.end()) {
            return iter.value();
        }
    }
    return false;
}

QString ServiceBase::name() const
{
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

QString ServiceBase::libPath() const
{
    if (m_policy) {
        return m_policy->m_libPath;
    }
    return "";
}

bool ServiceBase::isResident() const
{
    if (m_policy && m_policy->m_policyStartType == "Resident") {
        return true;
    }
    return false;
}

void ServiceBase::test() {}
