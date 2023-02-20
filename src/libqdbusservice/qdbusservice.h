#ifndef QDBUSSERVICE_H
#define QDBUSSERVICE_H

#include <QDBusConnection>
#include <QObject>

#if defined(EXPORT_LIBRARY)
#  define EXPORT_PLUGIN_LIBRARY Q_DECL_EXPORT // TODO
#else
#  define EXPORT_PLUGIN_LIBRARY Q_DECL_IMPORT
#endif

class QDBusServicePrivate;

class EXPORT_PLUGIN_LIBRARY QDBusService : public QObject
{
    Q_OBJECT
    QDBusServicePrivate *d_ptr;
    Q_DECLARE_PRIVATE(QDBusService)

public:
    explicit QDBusService(QObject *parent = nullptr);
    void InitPolicy(const QDBusConnection::BusType &busType, const QString &policyFile);
    QDBusConnection qDbusConnection() const;
};

#endif // QDBUSSERVICE_H
