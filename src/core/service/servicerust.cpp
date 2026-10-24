// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "servicerust.h"

#include <QByteArray>
#include <QFileInfo>
#include <QLibrary>
#include <QLoggingCategory>
#include <QMutexLocker>

Q_LOGGING_CATEGORY(dsm_service_rust, "[RustService]")

ServiceRust::ServiceRust(QObject *parent)
    : ServiceBase(parent)
    , m_library(nullptr)
    , m_start(nullptr)
    , m_stop(nullptr)
    , m_pluginHandle(nullptr)
{
    m_SDKType = SDKType::RUST;
}

ServiceRust::~ServiceRust()
{
    if (m_pluginHandle) {
        unregisterService();
    }
    delete m_library;
}

void ServiceRust::initThread()
{
    const QFileInfo fileInfo(QString(SERVICE_LIB_DIR) + policy->pluginPath);
    if (!QLibrary::isLibrary(fileInfo.absoluteFilePath())) {
        qCWarning(dsm_service_rust) << "invalid rust plugin:" << fileInfo.absoluteFilePath();
        return;
    }

    m_library = new QLibrary(fileInfo.absoluteFilePath());
    m_library->setLoadHints(QLibrary::ResolveAllSymbolsHint | QLibrary::PreventUnloadHint);
    if (!m_library->load()) {
        qCWarning(dsm_service_rust) << "failed to load rust plugin:"
                                    << fileInfo.absoluteFilePath() << m_library->errorString();
        return;
    }

    m_start = reinterpret_cast<DSMRustStartV1>(m_library->resolve("DSMRustStartV1"));
    m_stop = reinterpret_cast<DSMRustStopV1>(m_library->resolve("DSMRustStopV1"));
    if (!m_start || !m_stop) {
        qCWarning(dsm_service_rust) << "rust plugin ABI symbols not found:"
                                    << fileInfo.absoluteFilePath() << m_library->errorString();
        return;
    }

    if (!registerService()) {
        qCWarning(dsm_service_rust) << "register service failed:" << policy->name;
    }
    ServiceBase::initThread();
}

bool ServiceRust::registerService()
{
    QMutexLocker locker(&m_registerMutex);
    if (!m_start || m_pluginHandle) {
        return false;
    }

    const QByteArray serviceName = policy->name.toUtf8();
    const DSMRustPluginContextV1 context {
        DSM_RUST_PLUGIN_ABI_VERSION,
        sizeof(DSMRustPluginContextV1),
        m_sessionType == QDBusConnection::SessionBus ? DSMRustBusType::Session
                                                     : DSMRustBusType::System,
        0,
        serviceName.constData(),
        static_cast<std::size_t>(serviceName.size()),
    };

    void *pluginHandle = nullptr;
    const std::int32_t result = m_start(&context, &pluginHandle);
    if (result != 0 || !pluginHandle) {
        qCWarning(dsm_service_rust) << "DSMRustStartV1 failed:" << policy->name << result;
        return false;
    }

    m_pluginHandle = pluginHandle;
    return ServiceBase::registerService();
}

bool ServiceRust::unregisterService()
{
    QMutexLocker locker(&m_registerMutex);
    if (!m_pluginHandle) {
        return ServiceBase::unregisterService();
    }

    void *pluginHandle = m_pluginHandle;
    m_pluginHandle = nullptr;
    const std::int32_t result = m_stop(pluginHandle);
    if (result != 0) {
        qCWarning(dsm_service_rust) << "DSMRustStopV1 failed:" << policy->name << result;
        return false;
    }

    return ServiceBase::unregisterService();
}
