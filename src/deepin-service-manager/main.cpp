// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "pluginmanager.h"
#include "servicemanager.h"
#include "utils.h"

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QGuiApplication>
#include <QDebug>
#include <QLoggingCategory>
#include <QProcess>
#include <DLog>

#include <unistd.h>
Q_LOGGING_CATEGORY(dsm_Main, "[Main]")


int main(int argc, char *argv[])
{
    // 声明一个指向 QCoreApplication 或 QGuiApplication 的指针
    QCoreApplication *a;
    uid_t euid = geteuid();
    if (euid != 0) {
        a = new QGuiApplication(argc, argv);
    } else {
        a = new QCoreApplication(argc, argv);
    }
    
    Dtk::Core::DLogManager::registerConsoleAppender();
    Dtk::Core::DLogManager::registerJournalAppender();
    a->setApplicationVersion(VERSION);

    QCommandLineOption groupOption({ { "g", "group" }, "eg:core", "group name" });
    QCommandLineOption nameOption({ { "n", "name" }, "eg:org.deepin.demo", "service name" });
    QCommandLineParser parser;
    parser.setApplicationDescription("deepin service plugin loader");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOption(groupOption);
    parser.addOption(nameOption);
    parser.process(*a);

    const bool isSetGroup = parser.isSet(groupOption);
    const bool isSetName = parser.isSet(nameOption);

    const QString &typeValue = getuid() < 1000 ? "system" : "user";
    const QString &groupValue = isSetGroup ? parser.value(groupOption) : QString();
    const QString &nameValue = isSetName ? parser.value(nameOption) : QString();

    qCDebug(dsm_Main) << "deepin service config dir:" << QString(SERVICE_CONFIG_DIR);

    QMap<QString, QDBusConnection::BusType> busTypeMap;
    busTypeMap["system"] = QDBusConnection::SystemBus;
    busTypeMap["user"] = QDBusConnection::SessionBus;
    if (isSetName) {
        PluginManager *srv = new PluginManager();
        srv->init(busTypeMap[typeValue]);
        srv->loadByName(nameValue);
        QString newName = QString("service-manager: %1").arg(nameValue);
        setProcessName(argc, argv, newName.toStdString().c_str());
    } else if (isSetGroup) {
        PluginManager *srv = new PluginManager();
        srv->init(busTypeMap[typeValue]);
        srv->loadByGroup(groupValue);
        QString newName = QString("service-manager: %1").arg(groupValue);
        setProcessName(argc, argv, newName.toStdString().c_str());
    } else {
        ServiceManager *srv = new ServiceManager();
        srv->init(busTypeMap[typeValue]);
    }

    return a->exec();
}
