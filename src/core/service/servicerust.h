// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef SERVICERUST_H
#define SERVICERUST_H

#include "rustpluginabi.h"
#include "servicebase.h"

class QLibrary;

class ServiceRust : public ServiceBase
{
    Q_OBJECT
public:
    explicit ServiceRust(QObject *parent = nullptr);
    ~ServiceRust() override;

    bool registerService() override;
    bool unregisterService() override;

protected:
    void initThread() override;

private:
    QLibrary *m_library;
    DSMRustStartV1 m_start;
    DSMRustStopV1 m_stop;
    void *m_pluginHandle;
};

#endif // SERVICERUST_H
