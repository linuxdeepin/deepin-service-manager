#ifndef DDEQDBUSSERVICEPRIVATE_H
#define DDEQDBUSSERVICEPRIVATE_H

#include <QObject>

#include "service/serviceqtdbus.h"

class DDEQDBusServicePrivate : public ServiceQtDBus
{
    Q_OBJECT
public:
    explicit DDEQDBusServicePrivate(QObject *parent = nullptr);

    void InitPolicy(QDBusConnection::BusType busType, QString policyFile);

public Q_SLOTS:
    virtual void InitService();
};

#endif // DDEQDBUSSERVICEPRIVATE_H
