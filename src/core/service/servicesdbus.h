#ifndef SERVICESDBUS_H
#define SERVICESDBUS_H

#include "servicebase.h"

struct sd_bus;

class ServiceSDBus : public ServiceBase
{
    Q_OBJECT
public:
    ServiceSDBus(QObject *parent = nullptr);
    virtual ~ServiceSDBus();

    virtual bool registerService() override;

protected:
    virtual void initThread() override;

private:
    sd_bus *m_bus;
};

#endif // SERVICESDBUS_H
