#ifndef SERVICESDBUS_H
#define SERVICESDBUS_H

#include "servicebase.h"

struct sd_bus;
class QLibrary;

class ServiceSDBus : public ServiceBase
{
    Q_OBJECT
public:
    ServiceSDBus(QObject *parent = nullptr);
    virtual ~ServiceSDBus();

    virtual bool registerService() override;
    virtual bool unregisterService() override;

protected:
    virtual void initThread() override;

private:
    bool libFuncCall(const QString &funcName, bool isRegister);

private:
    sd_bus *m_bus;
    QLibrary *m_library;
};

#endif // SERVICESDBUS_H
