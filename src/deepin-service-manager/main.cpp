/*
 * Copyright (C) 2019 ~ 2022 Uniontech Technology Co., Ltd.
 *
 * Author:     fanpengcheng <fanpengcheng@uniontech.com>
 *
 * Maintainer: fanpengcheng <fanpengcheng@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "pluginmanager.h"
#include "servicemanager.h"

#include <qmap.h>

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    a.setApplicationVersion(VERSION);

    QCommandLineOption controlOption(
            { { "c", "control" }, "Format: <dbus type>.<group name>\neg:user.core", "control" });
    QCommandLineParser parser;
    parser.setApplicationDescription("deepin service manager");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOption(controlOption);
    parser.process(a);

    if (!parser.isSet(controlOption)) {
        qWarning() << "[main]must set dbus type!";
        return -1;
    }

    const QStringList &controlValues = parser.value(controlOption).split('.');
    const QString &typeValue = controlValues.first();
    const QString &groupValue = controlValues.last();
    const bool hasGroup = controlValues.count() == 2;

    if (!(typeValue == "system" || typeValue == "user")) {
        qWarning() << "[main]dbus type must be 'system' or 'user'!";
        return -1;
    }

    qDebug() << "[main]deepin service config dir:" << QString(SERVICE_CONFIG_DIR);

    QMap<QString, QDBusConnection::BusType> busTypeMap;
    busTypeMap["system"] = QDBusConnection::SystemBus;
    busTypeMap["user"] = QDBusConnection::SessionBus;
    QDBusConnection connection =
            typeValue == "user" ? QDBusConnection::sessionBus() : QDBusConnection ::systemBus();
    if (hasGroup) {
        PluginManager *srv = new PluginManager(connection);
        srv->init(busTypeMap[typeValue], groupValue);
    } else {
        ServiceManager *srv = new ServiceManager(connection);
        srv->init(busTypeMap[typeValue]);
    }

    return a.exec();
}
