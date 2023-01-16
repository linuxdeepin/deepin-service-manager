#include "servicemanagerprivate.h"

#include <QDebug>

ServiceManagerPrivate::ServiceManagerPrivate(QObject *parent)
    : QDBusService(parent)
{
}

ServiceManagerPrivate::~ServiceManagerPrivate() {}

void ServiceManagerPrivate::RegisterGroup(const QString &groupName,
                                          const QString &serviceName)
{
    qDebug() << "[ServiceManagerPrivate] register group:" << groupName
             << serviceName;
    emit requestRegisterGroup(groupName, serviceName);
}

void ServiceManagerPrivate::init(const QDBusConnection::BusType &type)
{
    QDBusService::InitPolicy(
        type,
        QString(SERVICE_CONFIG_DIR) + "other/qt-service/manager-private.json");
}
