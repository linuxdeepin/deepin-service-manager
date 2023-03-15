// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "servicesdbus.h"

#include <systemd/sd-bus.h>

#include <QDebug>
#include <QFileInfo>
#include <QLibrary>
#include <QThread>

int sd_bus_message_handler(sd_bus_message *m, void *userdata, sd_bus_error *ret_error)
{
    (void)ret_error;
    qInfo() << "[Hook-SDDBus]";
    std::string path = std::string(sd_bus_message_get_path(m));
    qInfo() << "[sd-bus hook]called path=" << QString::fromStdString(path);
    qInfo() << "[sd-bus hook]called interface=" << sd_bus_message_get_interface(m);
    qInfo() << "[sd-bus hook]called member=" << sd_bus_message_get_member(m);
    // sd_bus *bus = sd_bus_message_get_bus(m);
    qInfo() << "[sd-bus hook]called sender=" << sd_bus_message_get_sender(m);

    // int sd_bus_get_tid(sd_bus *bus, pid_t *tid);

    ServiceBase *qobj = static_cast<ServiceBase *>(userdata); // TODO 异常处理
    if (!qobj) {
        return -1;
    }

    if (!qobj->isRegister()) {
        qobj->registerService();
    }
    if (!qobj->policy->isResident()) {
        qInfo() << QString("--service: %1 will unregister in %2 minutes!")
                           .arg(qobj->policy->name)
                           .arg(qobj->policy->idleTime);
        qobj->restartTimer();
    }
    QString mem = sd_bus_message_get_member(m);
    if (mem == "Hello") {
        return sd_bus_reply_method_return(m, "s", "123");
        //        return -2; // -2: org.freedesktop.DBus.Error.FileNotFound:
    } else if (mem == "Introspect" && path == "/org/deepin/service/sdbus/demo1") {
        return sd_bus_reply_method_return(m, "s", "");
    }

    return 0;
}

ServiceSDBus::ServiceSDBus(QObject *parent)
    : ServiceBase(parent)
    , m_bus(nullptr)
{
    m_SDKType = SDKType::SD;
}

ServiceSDBus::~ServiceSDBus()
{
    if (this->thread() != nullptr) {
        this->thread()->quit();
        this->thread()->deleteLater();
    }
}

void ServiceSDBus::initThread()
{
    sd_bus_slot *slot = NULL;
    if (sd_bus_open_user(&m_bus) < 0) {
        qWarning() << "[ServiceSDBus]sd_bus_open_user error";
        return;
    }

    const char *unique;
    sd_bus_get_unique_name(m_bus, &unique);
    qInfo() << "[ServiceSDBus]bus unique:" << QString(unique);

    if (sd_bus_request_name(m_bus, policy->name.toStdString().c_str(), 0) < 0) {
        qWarning() << "[ServiceSDBus]sd_bus_request_name error";
        return;
    }

    if (sd_bus_add_filter(m_bus, &slot, sd_bus_message_handler, (void *)this) < 0) {
        qWarning() << "[ServiceSDBus]sd_bus_add_filter error";
        return;
    }

    const sd_bus_vtable calculator_vtable[] = { SD_BUS_VTABLE_START(0), SD_BUS_VTABLE_END };
    if (sd_bus_add_object_vtable(m_bus,
                                 &slot,
                                 "/PrivateDeclaration",
                                 "c.PrivateDeclaration", // TODO
                                 calculator_vtable,
                                 nullptr)
        < 0) {
        qWarning() << "[ServiceSDBus]sd_bus_add_object_vtable error";
        return;
    }

    QFileInfo fileInfo(QString(SERVICE_LIB_DIR) + policy->libPath);
    if (QLibrary::isLibrary(fileInfo.absoluteFilePath())) {
        m_library = new QLibrary(fileInfo.absoluteFilePath());
    }

    registerService();

    bool quit = false;
    while (!quit) { // TODO
        sd_bus_message *m = NULL;
        int r = sd_bus_process(m_bus, &m);
        qInfo() << "[ServiceSDBus]sd_bus_process finish and result=" << r;
        if (r < 0) {
            qWarning() << "[sd-bus hook]Failed to process requests: %m";
            break;
        }
        if (r == 0) {
            /* Wait for the next request to process */
            r = sd_bus_wait(m_bus, UINT64_MAX);
            if (r < 0) {
                qWarning() << "[ServiceSDBus]Failed to wait: %m";
                break;
            }
            continue;
        }
        if (!m) {
            continue;
        }
        qInfo() << "[ServiceSDBus]sd_bus_process Get msg=" << sd_bus_message_get_member(m);
        sd_bus_message_unref(m);
    }
    ServiceBase::initThread();
}

bool ServiceSDBus::registerService()
{
    if (m_bus == nullptr) {
        return false;
    }
    if (libFuncCall("DSMRegister", true)) {
        ServiceBase::registerService();
        return true;
    } else {
        return false;
    }
}

bool ServiceSDBus::unregisterService()
{
    if (libFuncCall("DSMUnRegister", false)) {
        ServiceBase::registerService();
        return true;
    } else {
        return false;
    }
}

bool ServiceSDBus::libFuncCall(const QString &funcName, bool isRegister)
{
    if (m_library == nullptr || !m_library->isLoaded()) {
        return false;
    }
    auto objFunc = isRegister ? DSMRegister(m_library->resolve(funcName.toStdString().c_str()))
                              : DSMUnRegister(m_library->resolve(funcName.toStdString().c_str()));
    if (!objFunc) {
        qWarning() << QString("[ServiceSDBus]failed to resolve the `%1` method: ").arg(funcName)
                   << m_library->fileName();
        if (m_library->isLoaded())
            m_library->unload();
        m_library->deleteLater();
        return false;
    }
    int ret = objFunc(policy->name.toStdString().c_str(), (void *)m_bus);
    if (ret) {
        return false;
    }
    return true;
}
