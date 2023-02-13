
#include "arealist.h"

void registerAreaListMetaType()
{
    qRegisterMetaType<MonitRect>("MonitRect");
    qDBusRegisterMetaType<MonitRect>();

    qRegisterMetaType<AreaList>("AreaList");
    qDBusRegisterMetaType<AreaList>();
}

QDBusArgument &operator<<(QDBusArgument &arg, const MonitRect &rect)
{
    arg.beginStructure();
    arg << rect.x1 << rect.y1 << rect.x2 << rect.y2;
    arg.endStructure();

    return arg;
}

const QDBusArgument &operator>>(const QDBusArgument &arg, MonitRect &rect)
{
    arg.beginStructure();
    arg >> rect.x1 >> rect.y1 >> rect.x2 >> rect.y2;
    arg.endStructure();

    return arg;
}
