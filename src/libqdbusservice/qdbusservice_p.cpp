#include "qdbusservice_p.h"

#include <QDebug>
#include <QMap>
#include <QVariant>

#include "policy/policy.h"
#include "service/qtdbushook.h"

QDBusServicePrivate::QDBusServicePrivate(QObject *parent)
    : ServiceQtDBus(parent)
{
    ServiceBase::registerService();
}

void QDBusServicePrivate::InitPolicy(QDBusConnection::BusType busType,
                                     QString policyFile)
{
    Policy *policy = new Policy(this);
    policy->parseConfig(policyFile);
    init(busType, policy);
}

void QDBusServicePrivate::InitService()
{
    qInfo() << "[QDBusServicePrivate]init service name:" << policy->name;
    QTDbusHook::instance()->setServiceObject(this);
}
