#include "servicemanagerpublic.h"

#include "libqdbusservice/qdbusservice.h"

ServiceManagerPublic::ServiceManagerPublic(QObject *parent)
    : QDBusService(parent)
{
}

ServiceManagerPublic::~ServiceManagerPublic() { }

void ServiceManagerPublic::addGroup(const QString &groupName)
{
    if (!m_groups.contains(groupName)) {
        m_groups.append(groupName);
    }
}

void ServiceManagerPublic::init(const QDBusConnection::BusType &type)
{
    QDBusService::InitPolicy(type, QString(SERVICE_CONFIG_DIR) + "other/manager.json");
}

QStringList ServiceManagerPublic::groups() const
{
    return m_groups;
}

QString ServiceManagerPublic::version() const
{
    return "1.0";
}
