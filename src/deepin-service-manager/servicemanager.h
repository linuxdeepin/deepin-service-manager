#ifndef SERVICEMANAGER_H
#define SERVICEMANAGER_H

#include <QDBusConnection>
#include <QMap>
#include <QObject>

class ServiceManagerPublic;
class ServiceManagerPrivate;
class GroupManager;

class ServiceManager : public QObject
{
    Q_OBJECT
public:
    explicit ServiceManager(QObject *parent = nullptr);
    ~ServiceManager();

    void init(const QDBusConnection::BusType &type);

private slots:
    void onRegisterGroup(const QString &groupName, const QString &serviceName);

private:
    void initConnection();
    void initGroup(const QDBusConnection::BusType &type);
    GroupManager *addGroup(const QString &group);

private:
    ServiceManagerPublic *m_publicService;
    ServiceManagerPrivate *m_privateService;
    QDBusConnection::BusType m_busType;
};

#endif // PLUGINMANAGER_H
