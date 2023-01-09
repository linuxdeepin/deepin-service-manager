#ifndef SERVICEMANAGERPRIVATE_H
#define SERVICEMANAGERPRIVATE_H

#include "libddeqdbusservice/ddeqdbusservice.h"
#include "utils.h"

class ServiceManagerPrivate : public DDEQDBusService
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", ServiceManagerInterface);

public:
    explicit ServiceManagerPrivate(QObject *parent = nullptr);
    ~ServiceManagerPrivate();

    void init(const QDBusConnection::BusType &type);

public slots:
    void RegisterGroup(const QString &groupName, const QString &serviceName);

signals:
    void requestRegisterGroup(const QString &groupName,
                              const QString &serviceName);
};

#endif  // PLUGINMANAGERPRIVATE_H
