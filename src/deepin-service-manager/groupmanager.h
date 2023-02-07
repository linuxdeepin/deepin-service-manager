#ifndef GROUPMANAGER_H
#define GROUPMANAGER_H

#include <QObject>

class GroupManager : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.deepin.service.manager1");
    Q_PROPERTY(QStringList Plugins READ plugins);

public:
    explicit GroupManager(QObject *parent = nullptr);

public Q_SLOTS:
    void addPlugin(const QString &pluginName);
    void removePlugin(const QString &pluginName);

private:
    QStringList plugins() const;

private:
    QStringList m_plugins;
};

#endif  // GROUPMANAGER_H
