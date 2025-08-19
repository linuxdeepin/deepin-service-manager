// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "policy.h"

#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(dsm_policy, "[Policy]")

Policy::Policy(QObject *parent)
    : QObject(parent)
    , dbus(nullptr)
{
}

bool Policy::checkPathHide(const QString &path)
{
    QMapPathHide::iterator iter = mapPathHide.find(path);
    if (iter == mapPathHide.end()) {
        return false;
    }
    return iter.value();
}

QStringList Policy::paths() const
{
    return mapSubPath.keys();
}

bool Policy::allowSubPath(const QString &path) const
{
    auto iter = mapSubPath.find(path);
    if (iter != mapSubPath.end()) {
        return iter.value();
    }
    return false;
}

bool Policy::isResident() const
{
    return startType == "Resident";
}

void Policy::print()
{
    qInfo() << "-------------------------------------";
    qInfo() << "DBUS POLICY CONFIG";
    qInfo() << "- name:" << name;
    qInfo() << "- path hide";
    for (QMapPathHide::iterator iter = mapPathHide.begin(); iter != mapPathHide.end(); iter++) {
        qInfo() << "-- path hide:" << iter.key() << iter.value();
    }
    qInfo() << "- policy";
    for (QMapPath::iterator iter = mapPath.begin(); iter != mapPath.end(); iter++) {
        qInfo() << "-- path:" << iter.key() << iter.value().path;
    }
    qInfo() << "-------------------------------------";
}

void Policy::parseConfig(const QString &path)
{
    qCInfo(dsm_policy) << "parse config:" << path;
    if (path.isEmpty()) {
        qCWarning(dsm_policy) << "path is empty!";
        return;
    }
    QJsonDocument jsonDoc;
    if (!readJsonFile(jsonDoc, path)) {
        qCWarning(dsm_policy) << "read json file failed!";
        return;
    }
    QJsonObject rootObj = jsonDoc.object();
    jsonGetString(rootObj, "name", name);
    jsonGetString(rootObj, "group", group, "app");
    jsonGetString(rootObj, "libPath", pluginPath); // 兼容
    jsonGetString(rootObj, "pluginPath", pluginPath, pluginPath);
    jsonGetString(rootObj, "policyVersion", version, "1.0"); // 兼容
    jsonGetString(rootObj, "version", version, version);
    jsonGetString(rootObj, "policyStartType", startType, "Resident"); // 兼容
    jsonGetString(rootObj, "startType", startType, startType);
    jsonGetStringList(rootObj, "dependencies", dependencies);
    jsonGetInt(rootObj, "startDelay", startDelay, 0);
    jsonGetInt(rootObj, "idleTime", idleTime, 10);
    // get SDKType
    QString sdkTypeString;
    jsonGetString(rootObj, "pluginType", sdkTypeString, "qt");
    if (sdkTypeString == "qt")
        sdkType = SDKType::QT;
    if (sdkTypeString == "sd")
        sdkType = SDKType::SD;

    if (name.isEmpty()) {
        qCWarning(dsm_policy) << "json error, name is empty.";
        return;
    }

    if (!parsePolicy(rootObj)) {
        qCWarning(dsm_policy) << "json error, parse policy error.";
        return;
    }
}

bool Policy::readJsonFile(QJsonDocument &outDoc, const QString &fileName)
{
    QFile jsonFIle(fileName);
    if (!jsonFIle.open(QIODevice::ReadOnly)) {
        qCWarning(dsm_policy) << QString("open file: %1 error!").arg(fileName);
        return false;
    }

    QJsonParseError jsonParserError;
    outDoc = QJsonDocument::fromJson(jsonFIle.readAll(), &jsonParserError);
    jsonFIle.close();
    if (jsonParserError.error != QJsonParseError::NoError) {
        qCWarning(dsm_policy) << "to json document error: " << jsonParserError.errorString();
        return false;
    }
    if (outDoc.isNull()) {
        qCWarning(dsm_policy) << "json document is null!";
        return false;
    }
    return true;
}

bool Policy::parsePolicy(const QJsonObject &obj)
{
    mapPathHide.clear();
    mapPath.clear();
    if (!obj.contains("policy")) {
        // 为空，不是出错
        return true;
    }
    QJsonValue policyValue = obj.value("policy");
    if (!policyValue.isArray()) {
        qCWarning(dsm_policy) << "parse policy error, must be json array!";
        return false;
    }
    QJsonArray policyList = policyValue.toArray();
    for (int i = 0; i < policyList.size(); ++i) {
        QJsonValue policy = policyList.at(i);
        if (!policy.isObject())
            continue;
        if (!parsePolicyPath(policy.toObject())) {
            return false;
        }
    }
    return true;
}

bool Policy::parsePolicyPath(const QJsonObject &obj)
{
    QString path;
    jsonGetString(obj, "path", path);
    if (path.isEmpty()) {
        qCWarning(dsm_policy) << "parse policy-path error, must be a string!";
        return false;
    }

    bool pathHide;
    jsonGetBool(obj, "pathhide", pathHide, false);
    mapPathHide.insert(path, pathHide);

    bool subpath;
    jsonGetBool(obj, "subpath", subpath, false);
    mapSubPath.insert(path, subpath);

    PolicyPath policyPath;
    policyPath.path = path;
    // Permission and whitelist parsing removed - process-based access control is disabled
    policyPath.needPermission = false;

    mapPath.insert(path, policyPath);

    return true;
}

bool Policy::jsonGetString(const QJsonObject &obj,
                           const QString &key,
                           QString &value,
                           QString defaultValue)
{
    if (obj.contains(key)) {
        const QJsonValue &v = obj.value(key);
        if (v.isString()) {
            value = v.toString();
            return true;
        }
    }
    value = defaultValue;
    return false;
}

bool Policy::jsonGetStringList(const QJsonObject &obj,
                               const QString &key,
                               QStringList &value,
                               QStringList defaultValue)
{
    value = defaultValue;
    if (!obj.contains(key))
        return false;
    const QJsonValue &v = obj.value(key);
    if (v.isString()) {
        value.append(v.toString());
        return true;
    }
    if (v.isArray()) {
        const QJsonArray &array = v.toArray();
        for (auto &&a : array) {
            if (a.isString())
                value.append(a.toString());
        }
    }
    return true;
}

bool Policy::jsonGetBool(const QJsonObject &obj, const QString &key, bool &value, bool defaultValue)
{
    if (obj.contains(key)) {
        const QJsonValue &v = obj.value(key);
        if (v.isBool()) {
            value = v.toBool();
            return true;
        }
    }
    value = defaultValue;
    return false;
}

bool Policy::jsonGetInt(const QJsonObject &obj, const QString &key, int &value, int defaultValue)
{
    if (obj.contains(key)) {
        const QJsonValue &v = obj.value(key);
        if (v.isDouble()) {
            value = v.toInt();
            return true;
        }
    }
    value = defaultValue;
    return false;
}
