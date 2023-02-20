#include "qdbusservice_p.h"

#include "policy/policy.h"
#include "service/qtdbushook.h"

#include <QDebug>
#include <QMap>
#include <QVariant>

QDBusServicePrivate::QDBusServicePrivate(QObject *parent)
    : ServiceQtDBus(parent)
{
    ServiceBase::registerService();
}

void QDBusServicePrivate::InitPolicy(QDBusConnection::BusType busType, QString policyFile)
{
    Policy *policy = new Policy(this);
    policy->parseConfig(policyFile);
    init(busType, policy);
}

void QDBusServicePrivate::InitService()
{
    qInfo() << "[QDBusServicePrivate]init service: " << policy->name;
    QTDbusHook::instance()->setServiceObject(this);
}

QDBusConnection QDBusServicePrivate::qDbusConnection() const
{
    if (policy->name.isEmpty()) {
        if (m_sessionType == QDBusConnection::SystemBus) {
            return QDBusConnection::systemBus();
        } else {
            return QDBusConnection::sessionBus();
        }
    } else {
        if (m_sessionType == QDBusConnection::SystemBus) {
            return QDBusConnection::connectToBus(QDBusConnection::SystemBus, policy->name);
        } else {
            return QDBusConnection::connectToBus(QDBusConnection::SessionBus, policy->name);
        }
    }
}
