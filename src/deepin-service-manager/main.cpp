// SPDX-FileCopyrightText: 2019 - 2026 UnionTech Software Technology Co., Ltd.
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

#include <systemd/sd-login.h>

#include <cstdlib>
#include <unistd.h>
#include <pwd.h>
Q_LOGGING_CATEGORY(dsm_Main, "[Main]")

static QString detectScopeBySystemd()
{
    /*
     * 此函数仅在未指定 --scope 时调用，按进程所属的 systemd/logind
     * 上下文自动判断服务级别：
     *
     * 1. 普通用户或 root 的 systemd --user 服务，以及用户直接登录后
     *    从终端启动的进程，都属于对应用户的登录会话或 user manager。
     *    sd_pid_get_owner_uid() 会返回该上下文的所有者 UID：
     *      - owner UID 与进程真实 UID 相同：按 user 级服务处理；
     *      - 普通用户通过 sudo/su 切换到 root 时，owner UID 通常仍是
     *        原登录用户，而进程真实 UID 已变为 0：按 system 级处理。
     *
     * 2. 由 system manager 启动的 unit（例如 /etc/systemd/system/ 下
     *    通过 systemctl start 启动的服务）不属于登录会话或 user
     *    manager，sd_pid_get_owner_uid() 通常返回 -ENODATA；随后
     *    sd_pid_get_unit() 会返回所属 system unit，因此按 system 级
     *    处理。unit 中配置 User=root 或 User=deepin-daemon 不会改变
     *    它仍是 system unit 这一事实。
     *
     * 3. 必须先判断 owner UID。user 服务同时嵌套在 system manager
     *    的 user@<uid>.service 中，如果先调用 sd_pid_get_unit()，
     *    可能把 systemd --user 服务误判成 system。
     *
     * 两个接口都无法判断时，在此按进程真实 UID 规则兜底。
     */
    const uid_t processUid = getuid();
    uid_t ownerUid = 0;
    if (sd_pid_get_owner_uid(0, &ownerUid) >= 0) {
        return ownerUid == processUid ? QStringLiteral("user") : QStringLiteral("system");
    }

    char *unit = nullptr;
    if (sd_pid_get_unit(0, &unit) >= 0) {
        std::free(unit);
        return QStringLiteral("system");
    }

    return processUid < 1000 ? QStringLiteral("system") : QStringLiteral("user");
}

int main(int argc, char *argv[])
{
    // 声明一个指向 QCoreApplication 或 QGuiApplication 的指针
    QCoreApplication *a;
    uid_t euid = geteuid();
    bool useCoreApp = (euid == 0);
    
    // 检查是否是deepin-daemon 用户
    if (!useCoreApp) {
        struct passwd *pw = getpwuid(euid);
        useCoreApp = (pw && QString::fromUtf8(pw->pw_name) == "deepin-daemon");
    }
    
    if (useCoreApp) {
        a = new QCoreApplication(argc, argv);
    } else {
        a = new QGuiApplication(argc, argv);
    }
    a->setApplicationName("org.deepin.service.manager");

    Dtk::Core::DLogManager::registerConsoleAppender();
    Dtk::Core::DLogManager::registerJournalAppender();
    a->setApplicationVersion(VERSION);

    QCommandLineOption groupOption({ { "g", "group" }, "eg:core", "group name" });
    QCommandLineOption nameOption({ { "n", "name" }, "eg:org.deepin.demo", "service name" });
    QCommandLineOption scopeOption({ "s", "scope" }, "service scope: system or user", "scope");
    QCommandLineOption elfQtVerCheckOption("elf-qt-version-check", "service manager plugin so <path>", "path");
    QCommandLineParser parser;
    parser.setApplicationDescription("deepin service plugin loader");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOption(groupOption);
    parser.addOption(nameOption);
    parser.addOption(scopeOption);
    parser.addOption(elfQtVerCheckOption);
    parser.addHelpOption();
    parser.process(*a);

    if (parser.isSet(elfQtVerCheckOption)) {
        QString filePath(parser.value(elfQtVerCheckOption));
        qCDebug(dsm_Main) << "Checking Qt version from ELF header of" << filePath;
        qCDebug(dsm_Main) << qtVersionFromSo(filePath);
        return 0;
    }

    const bool isSetGroup = parser.isSet(groupOption);
    const bool isSetName = parser.isSet(nameOption);

    QString typeValue;
    if (parser.isSet(scopeOption)) {
        typeValue = parser.value(scopeOption).toLower();
        if (typeValue != "system" && typeValue != "user") {
            qCCritical(dsm_Main) << "invalid scope:" << typeValue
                                 << "(expected system or user)";
            return 2;
        }
    } else {
        typeValue = detectScopeBySystemd();
    }

    qCDebug(dsm_Main) << "service scope:" << typeValue;
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
