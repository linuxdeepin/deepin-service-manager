#include "service.h"

Service::Service(QObject *parent)
    : QObject(parent)
{
}

QString Service::Hello()
{
    return "World";
}
