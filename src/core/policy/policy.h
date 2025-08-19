// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef POLICY_H
#define POLICY_H

#include <QDBusConnection>
#include <QMap>
#include <QObject>

enum class SDKType { QT, SD };

typedef QMap<QString, bool> QMapPathHide;
// subpath, default:false
typedef QMap<QString, bool> QMapSubPath;

struct PolicyPath
{
    QString path;
    bool needPermission;
};

typedef QMap<QString, PolicyPath> QMapPath;

class Policy : public QObject
{
    Q_OBJECT
public:
    explicit Policy(QObject *parent = nullptr);

    void parseConfig(const QString &path);

    bool checkPathHide(const QString &path);
    QStringList paths() const;
    bool allowSubPath(const QString &path) const;
    bool isResident() const;

    //    void Check(); // TODO
    void print();

private:
    bool readJsonFile(QJsonDocument &outDoc, const QString &fileName);
    bool parsePolicy(const QJsonObject &obj);
    bool parsePolicyPath(const QJsonObject &obj);

    bool jsonGetString(const QJsonObject &obj,
                       const QString &key,
                       QString &value,
                       QString defaultValue = "");
    bool jsonGetStringList(const QJsonObject &obj,
                           const QString &key,
                           QStringList &value,
                           QStringList defaultValue = {});
    bool
    jsonGetBool(const QJsonObject &obj, const QString &key, bool &value, bool defaultValue = false);
    bool jsonGetInt(const QJsonObject &obj, const QString &key, int &value, int defaultValue = 0);

public:
    // 数据定义
    // 主要功能：
    // - 路径隐藏: mapPathHide - 控制DBus路径是否在Introspect中显示
    // - 子路径支持: mapSubPath - 支持动态生成的子路径
    // - 基本路径信息: mapPath - 存储路径的基本配置信息
    QMapPathHide mapPathHide;
    QMapSubPath mapSubPath;
    QMapPath mapPath;

public:
    QString name;
    QString group;
    QString pluginPath;
    QString version;
    QString startType;
    QStringList dependencies;
    SDKType sdkType;
    int startDelay;
    int idleTime;
    QDBusConnection *dbus;
};

#endif // POLICY_H
