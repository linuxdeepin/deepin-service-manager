// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef SERVICE_H
#define SERVICE_H

#include <QObject>

class Service : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.deepin.OOMScoreAdjust")

public:
    explicit Service(QObject *parent = 0);

public Q_SLOTS:
    void InitOOMScoreAdjust();
};

#endif // SERVICE_H
