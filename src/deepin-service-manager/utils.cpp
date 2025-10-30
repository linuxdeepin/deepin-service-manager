// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include  "utils.h"

#include <QPluginLoader>
#include <QLibrary>
#include <QVersionNumber>
#include <QLoggingCategory>

#include <cstring>
#include <sys/prctl.h>

Q_LOGGING_CATEGORY(dsm_utils, "[Utils]")

void setProcessName(int argc, char **argv, const char *title)
{
    qCDebug(dsm_utils) << "Setting process name to:" << title;

    size_t titleLen = strlen(title);
    size_t argvMaxLen = argv[argc - 1] + strlen(argv[argc - 1]) - argv[0];
    if (titleLen >= argvMaxLen) {
        qCWarning(dsm_utils) << "Process name too long, truncating:" << titleLen << ">" << (argvMaxLen - 1);
        strncpy(argv[0], title, argvMaxLen - 1);
        argv[0][argvMaxLen - 1] = '\0';
    } else {
        strcpy(argv[0], title);
        memset(argv[0] + titleLen, '\0', argvMaxLen - titleLen);
    }

    if (prctl(PR_SET_NAME, title, 0, 0, 0) != 0) {
        qCWarning(dsm_utils) << "Failed to set process name via prctl:" << strerror(errno);
    } else {
        qCDebug(dsm_utils) << "Process name set successfully to:" << title;
    }
}

bool checkLibraryQtVersion(const QString &soPath)
{
    QLibrary library(soPath);
    if (!library.load()) {
        qCWarning(dsm_utils) << "Error opening library:" << soPath << library.errorString();
        return false;
    }

    // 尝试解析Qt版本相关的符号
    using QVersionFunc = const char* (*)();
    auto qVersionFunc = reinterpret_cast<QVersionFunc>(library.resolve("qVersion"));
    if (!qVersionFunc) {
        qCWarning(dsm_utils) << "This library does not appear to link against Qt.";
        return false;
    }

    // 如果找到了qVersion符号，那么这个库确实链接了Qt
    const char *qtVersion = qVersionFunc();
    qCInfo(dsm_utils) << "Library Qt version:" << qtVersion;

    QVersionNumber libraryQtVersion = QVersionNumber::fromString(QString::fromUtf8(qtVersion));
    QVersionNumber expectedQtVersion = QVersionNumber::fromString(QString::fromUtf8(&USE_QT_VERSION_MAJOR));

    // 检查主版本号
    if (libraryQtVersion.majorVersion() == expectedQtVersion.majorVersion()) {
        return true;
    }

    qCWarning(dsm_utils) << "This library links against Qt" << libraryQtVersion.toString()
                         << "but expected Qt" << expectedQtVersion.majorVersion();
    return false;
}
