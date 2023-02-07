#ifndef SERVICEQTDBUS_H
#define SERVICEQTDBUS_H

#include "servicebase.h"

class ServiceQtDBus : public ServiceBase
{
    Q_OBJECT
public:
    explicit ServiceQtDBus(QObject *parent = nullptr);

    QDBusConnection qDbusConnection();

    virtual bool registerService() override;

protected:
    virtual void initThread() override;
};

#endif  // SERVICEQTDBUS_H
