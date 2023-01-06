#include "pluginconfig.h"

#include <QSettings>
#include <QFile>
#include <QDebug>

PluginConfig::PluginConfig()
{
    QString ss("aaa , vvv, bbb ");
    QStringList sl = ss.split(",");
    qInfo() << sl;
    qInfo() << sl.at(0).simplified() << ",len:" << sl.at(0).simplified().length();
    qInfo() << sl.at(1).simplified()<< ",len:" << sl.at(1).simplified().length();
    qInfo() << sl.at(2).simplified() << ",len:" << sl.at(2).simplified().length();
}

bool PluginConfig::loadFile(QString fileName)
{
    QFile file(fileName);
    if (!file.exists()) {
        qWarning() << "plugin config load failed: find is not existed.";
        return false;;
    }
    QSettings settings(fileName, QSettings::IniFormat);
    if (settings.status() != QSettings::NoError) {
        qWarning() << "plugin config load failed: invalid format.";
        return false;
    }
    settings.beginGroup("plugin");
    m_pluginVersion = settings.value("version").toString();
    m_startType = settings.value("startType").toString();
    m_libPath = settings.value("libPath").toString();
    settings.endGroup();
    settings.beginGroup("dbus");
    m_SDKType = settings.value("SDKType").toString();
    m_dbusName = settings.value("name").toString();
    m_dbusPath = settings.value("path").toString();
    m_dbusInterface = settings.value("interface").toString();
    m_policyPath = settings.value("policyPath").toString();
    settings.endGroup();
    //    print();
    return true;
}

bool PluginConfig::loadData(const QMap<QString, QVariant> &data)
{
    QMap<QString, QVariant>::const_iterator iter = data.begin();
    for (; iter != data.end(); ++iter) {
        const QString &key = iter.key();
        if (key == "version") {
            m_pluginVersion = iter.value().toString();
        } else if (key == "startType") {
            m_startType = iter.value().toString();
        } else if (key == "libPath") {
            m_libPath = iter.value().toString();
        } else if (key == "SDKType") {
            m_SDKType = iter.value().toString();
        } else if (key == "name") {
            m_dbusName = iter.value().toString();
        } else if (key == "path") {
            m_dbusPath = iter.value().toString();
        } else if (key == "interface") {
            m_dbusInterface = iter.value().toString();
        } else if (key == "policyPath") {
            m_policyPath = iter.value().toString();
        }
    }
    return true;
}

bool PluginConfig::check()
{
    // TODO

//    if (info.version.isEmpty() || !CompatiblePluginApiList.contains(info.version)) {
//        qCDebug(plugin) << "plugin api version not matched! expect versions:" << CompatiblePluginApiList
//                 << ", got version:" << info.version
//                 << ", the plugin file is:" << fileInfo.absoluteFilePath();
//        if (lib->isLoaded())
//            lib->unload();
//        lib->deleteLater();
//        return;
//    }
    return true;
}

void PluginConfig::print()
{
    qInfo() << "[Plugin Config Print]";
    qInfo() << "version:" << m_pluginVersion;
    qInfo() << "startType:" << m_startType;
    qInfo() << "libPath:" << m_libPath;
    qInfo() << "SDKType:" << m_SDKType;
    qInfo() << "name:" << m_dbusName;
    qInfo() << "path:" << m_dbusPath;
    qInfo() << "interface:" << m_dbusInterface;
    qInfo() << "policyPath:" << m_policyPath;
}
