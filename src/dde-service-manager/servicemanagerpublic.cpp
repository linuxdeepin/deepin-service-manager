#include "servicemanagerpublic.h"

#include "libddeqdbusservice/ddeqdbusservice.h"

ServiceManagerPublic::ServiceManagerPublic(QObject *parent)
    : DDEQDBusService(parent)
{
}

ServiceManagerPublic::~ServiceManagerPublic() {}

void ServiceManagerPublic::addGroup(const QString &groupName)
{
    if (!m_groups.contains(groupName)) {
        m_groups.append(groupName);
    }
}

void ServiceManagerPublic::init(const QDBusConnection::BusType &type)
{
    DDEQDBusService::InitPolicy(type,
                                QString(DEEPIN_SERVICE_MANAGER_DIR) +
                                    "other/dde-qt-service/manager-public.json");
}

QStringList ServiceManagerPublic::groups() const
{
    return m_groups;
}

QString ServiceManagerPublic::version() const
{
    return "1.0";
}
