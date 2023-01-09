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
#include <QCoreApplication>
#include <QDebug>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDBusConnection>
#include <QDBusError>

#include "servicemanager.h"

//int main(int argc, char *argv[])
//{
//#ifndef QT_DEBUG
//    if (argc != 2) {
//        qWarning() << "you need set run parameters";
//        return -1;
//    }

//    const QString &command = QString(argv[1]);
//#else
//    const QString &command = "--session";
//#endif  //QT_DEBUG

//    if (command == "--session") {
//        QApplication a(argc, argv);
//        a.setOrganizationName("deepin");
//        a.setApplicationName("dde-service-manager");

//        DBusManager *manager = DBusManager::instance(SessionType::Session);
//        const QString &interface = manager->metaObject()->classInfo(0).value();
//        const QString &path = manager->metaObject()->classInfo(1).value();

//        if (!QDBusConnection::sessionBus().registerService(interface)) {
//            qDebug() << "DBus register failed, error message:" << QDBusConnection::sessionBus().lastError().message();
//            exit(-1);
//        }
//        QDBusConnection::sessionBus().registerObject(path, manager, QDBusConnection::ExportAllSlots | QDBusConnection::ExportAllSignals);
//        return a.exec();
//    }

//    if (command == "--system") {
//        QCoreApplication a(argc, argv);
//        a.setOrganizationName("deepin");
//        a.setApplicationName("dde-service-manager");

//        DBusManager *manager = DBusManager::instance(SessionType::System);
//        const QString &interface = manager->metaObject()->classInfo(0).value();
//        const QString &path = manager->metaObject()->classInfo(1).value();

//        if (!QDBusConnection::systemBus().registerService(interface)) {
//            qDebug() << "DBus register failed,  error message:" << QDBusConnection::systemBus().lastError().message();
//            exit(-1);
//        }
//        QDBusConnection::systemBus().registerObject(path, manager, QDBusConnection::ExportAllSlots | QDBusConnection::ExportAllSignals);
//        return a.exec();
//    }

//    return -2;
//}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

#ifdef QT_DEBUG
    qInfo() << "deepin service manager dir:" << QString(DEEPIN_SERVICE_MANAGER_DIR);
#endif

    ServiceManager *srv = new ServiceManager();
    srv->serverInit(SessionType::Session);
    if (!QDBusConnection::sessionBus().registerObject(ServiceManagerDBusPath, srv, QDBusConnection::ExportAllSlots | QDBusConnection::ExportAllSignals)) {
        qWarning() << "failed to register dbus object" << QDBusConnection::sessionBus().lastError().message();
    }
    if (!QDBusConnection::sessionBus().registerService(ServiceManagerDBusName)) {
        qWarning() << "failed to register dbus object" << QDBusConnection::sessionBus().lastError().message();
    }

    return a.exec();
}
