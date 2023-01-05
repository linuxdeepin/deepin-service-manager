#pragma once

#include <QString>
#include <QMap>
#include <QVariant>

class PluginConfig {

public:
    explicit PluginConfig();

    bool loadFile(QString fileName);
    bool loadData(const QMap<QString, QVariant> &data);

    bool check();
    void print();


// private: // TODO
    QString m_pluginVersion;
    QString m_startType; // Resident, OnDemandByDbus
    QString m_libPath;

    QString m_SDKType; // qtdbus、sdbus
    QString m_dbusName;
    QString m_dbusPath;
    QString m_dbusInterface;
    QString m_policyPath;
};
