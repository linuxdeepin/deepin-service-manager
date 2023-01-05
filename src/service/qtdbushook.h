#ifndef QTDBUSHOOK_H
#define QTDBUSHOOK_H

#include "service/servicebase.h"

typedef QMap<QString, ServiceBase*> ServiceObjectMap;

class QTDbusHook
{
public:
    explicit QTDbusHook();

    bool getServiceObject(QString name, QString path, ServiceBase **service, bool &isSubPath, QString &realPath);

    bool setServiceObject(ServiceBase *obj);

    static QTDbusHook* instance();

private:
    ServiceObjectMap m_serviceMap;
};

#endif // QTDBUSHOOK_H
