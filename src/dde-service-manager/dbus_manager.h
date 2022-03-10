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
#ifndef DBUSMANAGER_H
#define DBUSMANAGER_H

#include <QObject>
#include <QMap>
#include <QDBusContext>

enum SessionType{
    System,
    Session,
    Unknown
};

class QLibrary;
class DBusManager : public QObject, public QDBusContext
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.deepin.dde.ServiceManager")
    Q_CLASSINFO("D-Bus Path", "/com/deepin/dde/ServiceManager")

public:
    static DBusManager *instance(SessionType type = Session, QObject *parent = nullptr);

public Q_SLOTS:
    void Load(const QString &fileName);
    void UnLoad(const QString &name);
    bool IsRunning(const QString &name);
    QStringList PluginList();

Q_SIGNALS:
    void PluginLoaded(const QString &name);
    void PluginUnLoaded(const QString &name);

private:
    DBusManager(SessionType type = Session, QObject *parent = nullptr);
    ~ DBusManager();
    struct PluginInfo{
        QString name;
        QString version;
        int session_type = -1;
        QString service;
    };

    const QString getPluginPath(SessionType type);
    void resolveInfo(const char *data, PluginInfo &info);

private:
    static DBusManager *m_instance;
    QMap<QString /*name*/, QPair<QString/*fileName*/, QLibrary * /*lib*/>> m_map;
    SessionType m_type;
};

#endif // DBUSMANAGER_H
