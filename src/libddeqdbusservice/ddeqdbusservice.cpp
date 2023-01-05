#include "ddeqdbusservice.h"
#include "ddeqdbusserviceprivate.h"



DDEQDBusService::DDEQDBusService(QObject *parent) : QObject(parent)
    , d_ptr(new DDEQDBusServicePrivate(this))
{
}

void DDEQDBusService::InitPolicy(QDBusConnection::BusType busType, QString policyFile)
{
    Q_D(DDEQDBusService);
    d->InitPolicy(busType, policyFile);
}




