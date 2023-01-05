#ifndef SERVICESDBUS_H
#define SERVICESDBUS_H

#include "service/servicebase.h"
#include <systemd/sd-bus.h>

class ServiceSDBus : public ServiceBase
{
    Q_OBJECT
public:
    ServiceSDBus(QObject *parent = nullptr);
    virtual ~ServiceSDBus();

    virtual bool Register();

public Q_SLOTS:
    virtual void InitService();

    void InitThread();

private:
    sd_bus *m_bus;
};

#endif // SERVICESDBUS_H
