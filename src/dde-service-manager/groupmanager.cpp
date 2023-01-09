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

