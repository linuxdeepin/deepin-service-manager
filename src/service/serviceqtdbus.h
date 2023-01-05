#ifndef SERVICEQTDBUS_H
#define SERVICEQTDBUS_H

#include "service/servicebase.h"

class ServiceQtDBus : public ServiceBase
{
    Q_OBJECT
public:
    explicit ServiceQtDBus(QObject *parent = nullptr);

    QDBusConnection qDbusConnection();

    virtual bool Register();

public Q_SLOTS:
    virtual void InitService();
    virtual void InitThread();
};

#endif // SERVICEQTDBUS_H
