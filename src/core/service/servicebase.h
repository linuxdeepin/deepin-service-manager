#ifndef SERVICEBASE_H
#define SERVICEBASE_H

#include <QDBusConnection>
#include <QObject>

class Policy;
enum class SDKType { QT, SD };

typedef void *(*ServiceObject)(const char *path, const int len);
typedef int (*DSMRegisterObject)(const char *name, void *data);

class ServiceBase : public QObject
{
    Q_OBJECT
public:
    explicit ServiceBase(QObject *parent = nullptr);
    virtual ~ServiceBase();

    bool IsRegister() const;
    bool CheckMethodPermission(const QString &process,
                               const QString &path,
                               const QString &interface,
                               const QString &method) const;
    bool CheckPropertyPermission(const QString &process,
                                 const QString &path,
                                 const QString &interface,
                                 const QString &property) const;
    bool CheckPathHide(const QString &path) const;
    virtual bool Register();

    Qt::HANDLE threadID();
    QStringList paths() const;
    bool allowSubPath(const QString &path) const;
    QString name() const;
    QString group() const;
    QString libPath() const;
    bool isResident() const;

signals:

public Q_SLOTS:
    void test();

    void Init(const QDBusConnection::BusType &busType,
              const QString &configPath);
    virtual void InitService();

protected:
    bool m_isRegister;

    Policy *m_policy;

    QDBusConnection::BusType m_sessionType;
    SDKType m_SDKType;  // qtdbus、sdbus
};

#endif  // SERVICEBASE_H
