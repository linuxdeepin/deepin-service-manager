#include "servicemanagerprivate.h"

#include <QDebug>

ServiceManagerPrivate::ServiceManagerPrivate(QObject *parent)
    : DDEQDBusService(parent)
{
}

ServiceManagerPrivate::~ServiceManagerPrivate() {}

void ServiceManagerPrivate::RegisterGroup(const QString &groupName,
                                          const QString &serviceName)
{
    qDebug() << "[ServiceManagerPrivate] register group:" << groupName << serviceName;
    emit requestRegisterGroup(groupName, serviceName);
}

void ServiceManagerPrivate::init(const QDBusConnection::BusType &type)
{
    DDEQDBusService::InitPolicy(
        type,
        QString(DEEPIN_SERVICE_MANAGER_DIR) +
            "other/dde-qt-service/manager-private.json");
}
