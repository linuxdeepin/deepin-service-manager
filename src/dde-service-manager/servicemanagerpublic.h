#ifndef SERVICEMANAGERPUBLIC_H
#define SERVICEMANAGERPUBLIC_H

#include "libddeqdbusservice/ddeqdbusservice.h"

class ServiceManagerPublic : public DDEQDBusService
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.deepin.service.manager1");
    Q_PROPERTY(QStringList Groups READ groups);
    Q_PROPERTY(QString Version READ version);

public:
    explicit ServiceManagerPublic(QObject *parent = nullptr);
    ~ServiceManagerPublic();

    void addGroup(const QString &groupName);
    void init(const QDBusConnection::BusType &type);

private:
    QStringList groups() const;
    QString version() const;

private:
    QStringList m_groups;
};

#endif  // PLUGINMANAGERPUBLIC_H
