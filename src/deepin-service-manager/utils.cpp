// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include  "utils.h"
#include <cstring>
#include <sys/prctl.h>

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
