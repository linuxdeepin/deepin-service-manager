#include "ddeqdbusserviceprivate.h"

#include <QMap>
#include <QVariant>
#include <QDebug>

#include "service/qtdbushook.h"

DDEQDBusServicePrivate::DDEQDBusServicePrivate(QObject *parent) : ServiceQtDBus(parent)
{
    ServiceBase::Register();
}

void DDEQDBusServicePrivate::InitPolicy(QDBusConnection::BusType busType, QString policyFile)
{
    if (busType == QDBusConnection::SessionBus) {
        Init(SessionType::Session, policyFile);
    } else {
        Init(SessionType::System, policyFile);
    }
}

void DDEQDBusServicePrivate::InitService()
{
    qInfo() << "[DDEQDBusServicePrivate]init service:" << name();
    QTDbusHook::instance()->setServiceObject(this);

//    m_policy = new Policy(this);
//    m_policy->ParseConfig(policyPath());
//    m_policy->Print(); // TODO
}
