#ifndef SERVICEBASE_H
#define SERVICEBASE_H

#include "policy/policy.h"

#include <QObject>
#include <QDBusConnection>


enum class SDKType {QT, SD};

typedef void *(*ServiceObject)(const char *path, const int len);
typedef int (*DSMRegisterObject)(const char *name, void *data);

class ServiceBase : public QObject
{
    Q_OBJECT
public:
    explicit ServiceBase(QObject *parent = nullptr);
    virtual ~ServiceBase();

//    bool InitConfig(QString path);

    bool IsRegister();
    bool CheckMethodPermission(QString process, QString path, QString interface, QString method);
    bool CheckPropertyPermission(QString process, QString path, QString interface, QString property);
    bool CheckPathHide(QString path);
    virtual bool Register();

    Qt::HANDLE threadID();
    QStringList paths();
    bool allowSubPath(QString path);
    QString name();
    QString group() const;
//    QString interface();
    QString libPath();
//    QString policyPath();
    bool isResident();

signals:

public Q_SLOTS:
    void test();

    void Init(const QDBusConnection::BusType busType, const QString &configPath);
//    void InitByData(SessionType sessionType, const QMap<QString, QVariant> &data);
    virtual void InitService();

protected:

    bool m_isRegister;

    Policy *m_policy;

    QDBusConnection::BusType m_sessionType;
    SDKType m_SDKType; // qtdbus、sdbus
};

#endif // SERVICEBASE_H
