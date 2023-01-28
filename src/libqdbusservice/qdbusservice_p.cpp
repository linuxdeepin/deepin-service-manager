#include "qdbusservice_p.h"

#include <QDebug>
#include <QMap>
#include <QVariant>

#include "service/qtdbushook.h"

QDBusServicePrivate::QDBusServicePrivate(QObject *parent)
    : ServiceQtDBus(parent)
{
    ServiceBase::Register();
}

void QDBusServicePrivate::InitPolicy(QDBusConnection::BusType busType,
                                     QString policyFile)
{
    Init(busType, policyFile);
}

void QDBusServicePrivate::InitService()
{
    qInfo() << "[QDBusServicePrivate]init service name:" << name();
    QTDbusHook::instance()->setServiceObject(this);
}
