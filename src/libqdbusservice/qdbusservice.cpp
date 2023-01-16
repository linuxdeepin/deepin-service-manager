#include "qdbusservice.h"

#include "qdbusservice_p.h"

QDBusService::QDBusService(QObject *parent)
    : QObject(parent)
    , d_ptr(new QDBusServicePrivate(this))
{
}

void QDBusService::InitPolicy(QDBusConnection::BusType busType,
                              QString policyFile)
{
    Q_D(QDBusService);
    d->InitPolicy(busType, policyFile);
}
