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
#include "servicemanager.h"

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>

#include <unistd.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    a.setApplicationVersion(VERSION);

    const QString &typeValue = getuid() < 1000 ? "system" : "user";

    qDebug() << "[main]deepin service config dir:" << QString(SERVICE_CONFIG_DIR);

    QMap<QString, QDBusConnection::BusType> busTypeMap;
    busTypeMap["system"] = QDBusConnection::SystemBus;
    busTypeMap["user"] = QDBusConnection::SessionBus;
    ServiceManager *srv = new ServiceManager();
    srv->init(busTypeMap[typeValue]);

    return a.exec();
}
