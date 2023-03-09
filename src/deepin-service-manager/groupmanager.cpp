// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "groupmanager.h"

GroupManager::GroupManager(QObject *parent)
    : QObject(parent)
{
}

QStringList GroupManager::plugins() const
{
    return m_plugins;
}

void GroupManager::addPlugin(const QString &pluginName)
{
    m_plugins.append(pluginName);
}

void GroupManager::removePlugin(const QString &pluginName)
{
    if (m_plugins.contains(pluginName))
        m_plugins.removeOne(pluginName);
}
