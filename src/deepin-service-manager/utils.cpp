// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include  "utils.h"

#include <cstring>
#include <sys/prctl.h>
#include <iostream>
#include <string>
#include <dlfcn.h>

void setProcessName(int argc, char **argv, const char *title)
{
    size_t titleLen = strlen(title);
    size_t argvMaxLen = argv[argc - 1] + strlen(argv[argc - 1]) - argv[0];
    if (titleLen >= argvMaxLen) {
        strncpy(argv[0], title, argvMaxLen - 1);
        argv[0][argvMaxLen - 1] = '\0';
    } else {
        strcpy(argv[0], title);
        memset(argv[0] + titleLen, '\0', argvMaxLen - titleLen);
    }

    prctl(PR_SET_NAME, title, 0, 0, 0);
}

bool checkLibraryQtVersion(const QString &soPath)
{
    void *handle = dlopen(soPath.toStdString().c_str(), RTLD_LAZY);
    if (!handle) {
        qWarning() << "Error opening library: " << soPath << dlerror();
        return false;
    }

    // 尝试解析Qt版本相关的符号
    void *qtVersionSymbol = dlsym(handle, "qVersion");
    if (!qtVersionSymbol) {
        qWarning() << "This library does not appear to link against Qt.";
        dlclose(handle);
        return false;
    }

    // 如果找到了qVersion符号，那么这个库确实链接了Qt
    using QVersionFunc = const char* (*)();
    auto qVersionFunc = reinterpret_cast<QVersionFunc>(qtVersionSymbol);

    const char *qtVersion = qVersionFunc();
    std::cout << "Qt version: " << qtVersion << std::endl;

    // 检查主版本号
    if (qtVersion[0] == USE_QT_VERSION_MAJOR) {
        dlclose(handle);
        return true;
    }

    qWarning() << "This library links against a Qt version other than " << USE_QT_VERSION_MAJOR << ".";
    dlclose(handle);
    return false;
}
