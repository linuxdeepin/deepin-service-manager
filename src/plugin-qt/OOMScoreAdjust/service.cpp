// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "service.h"
#include <qlogging.h>

#include <DConfig>

#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDebug>
#include <QStringList>

#include <sys/stat.h>
#include <unistd.h>

static QString readNamespaceLinkTarget(const QString &path)
{
    char target[PATH_MAX];
    ssize_t len;

    len = readlink(path.toStdString().c_str(), target, sizeof(target) - 1);
    if (len == -1) {
        qWarning() << "Failed to read link" << path << ":" << strerror(errno);
        return "";
    }

    target[len] = '\0';
    return QString::fromStdString(std::string(target));
}

static QList<int> getPidByName(const QString &processName)
{
    QList<int> pids;
    const QString &systemdMnt = readNamespaceLinkTarget("/proc/1/ns/mnt");
    if (systemdMnt.isEmpty()) {
        qWarning() << "Failed to get systemd mount namespace";
        return pids;
    }
    QDir procDir("/proc");
    // 遍历 /proc 目录下的所有子目录（即所有的 PID）
    const QStringList &entries = procDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QString &entry : entries) {
        bool ok;
        int pid = entry.toInt(&ok);
        if (!ok) {
            continue; // 不是有效的 PID
        }

        const QString &exe = QString("/proc/%1/exe").arg(pid);
        if (!QFileInfo(exe).isSymLink() || !QFileInfo::exists(exe))
            continue;
        const QString &exeFilePath = QFileInfo(exe).symLinkTarget();
        if (exeFilePath != processName)
            continue;
        // 检查是否在 systemd 的 mount namespace 中
        const QString &mnt = readNamespaceLinkTarget(QString("/proc/%1/ns/mnt").arg(pid));
        if (mnt == systemdMnt) {
            pids << pid;
            qDebug() << "Found process:" << processName << "with PID:" << pid;
        }
    }

    return pids;
}

static int setOOMScoreAdj(int pid)
{
    // 构造 /proc/<pid>/oom_score_adj 的路径
    const QString &procPath = QDir::toNativeSeparators(QString("/proc/%1/oom_score_adj").arg(pid));
    QFile file(procPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to open" << procPath << "for writing:" << file.errorString();
        return -1;
    }

    // 写入 oom_score_adj 值
    QTextStream out(&file);
    out << -999;
    file.close();

    // 检查写入是否成功
    if (out.status() != QTextStream::Ok) {
        qWarning() << "Failed to write to" << procPath << ":" << out.status();
        return -1;
    }

    return 0;
}

Service::Service(QObject *parent)
    : QObject(parent)
{
    InitOOMScoreAdjust();
}

void Service::InitOOMScoreAdjust()
{
    auto config = Dtk::Core::DConfig::create("org.deepin.service.manager", "org.deepin.service.manager.oom-score-adjust", "", this);
    if (!config || !config->isValid()) {
        qWarning() << "org.deepin.service.manager.oom-score-adjust is not valid";
        return;
    }

    if (!config->value("enabled").toBool()) {
        qWarning() << "org.deepin.OOMScoreAdjust is disabled";
        return;
    }
    const QStringList &protectionProcessList = config->value("protectionProcess").toStringList();
    // 修改配置中OOMScoreAdjust的值
    for (const QString &process : protectionProcessList) {
        const QList<int> &pids = getPidByName(process);
        if (pids.size() > 0) {
            for (int processPid : pids) {
                int res = setOOMScoreAdj(processPid);
                if (res != 0) {
                    qWarning() << "Failed to set oom_score_adj for process" << process << processPid;
                }
            }
        } else {
            qWarning() << "Failed to find the pid for process" << process;
        }
    }

    config->deleteLater();
}
