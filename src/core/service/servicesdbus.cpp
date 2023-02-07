#include "servicesdbus.h"

#include <QDebug>
#include <QFileInfo>
#include <QLibrary>
#include <QThread>

#include <systemd/sd-bus.h>

int sd_bus_message_handler(sd_bus_message *m,
                           void *userdata,
                           sd_bus_error *ret_error)
{
    (void)ret_error;
    qInfo() << "[Hook-SDDBus]";
    std::string path = std::string(sd_bus_message_get_path(m));
    qInfo() << "[sd-bus hook]called path=" << QString::fromStdString(path);
    qInfo() << "[sd-bus hook]called interface="
            << sd_bus_message_get_interface(m);
    qInfo() << "[sd-bus hook]called member=" << sd_bus_message_get_member(m);
    // sd_bus *bus = sd_bus_message_get_bus(m);
    qInfo() << "[sd-bus hook]called sender=" << sd_bus_message_get_sender(m);

    // int sd_bus_get_tid(sd_bus *bus, pid_t *tid);

    ServiceBase *qobj = static_cast<ServiceBase *>(userdata);  // TODO 异常处理
    if (!qobj) {
        return -1;
    }

    if (!qobj->isRegister()) {
        qobj->registerService();
    }
    QString mem = sd_bus_message_get_member(m);
    if (mem == "Hello") {
        return sd_bus_reply_method_return(m, "s", "123");
        //        return -2; // -2: org.freedesktop.DBus.Error.FileNotFound:
    } else if (mem == "Introspect" &&
               path == "/org/deepin/service/sdbus/demo1") {
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

    if (sd_bus_add_filter(m_bus, &slot, sd_bus_message_handler, (void *)this) <
        0) {
        qWarning() << "[ServiceSDBus]sd_bus_add_filter error";
        return;
    }

    const sd_bus_vtable calculator_vtable[] = {SD_BUS_VTABLE_START(0),
                                               SD_BUS_VTABLE_END};
    if (sd_bus_add_object_vtable(m_bus,
                                 &slot,
                                 "/PrivateDeclaration",
                                 "c.PrivateDeclaration",  // TODO
                                 calculator_vtable,
                                 nullptr) < 0) {
        qWarning() << "[ServiceSDBus]sd_bus_add_object_vtable error";
        return;
    }

    if (policy->isResident()) {
        registerService();
    }

    bool quit = false;
    while (!quit) {  // TODO
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
        qInfo() << "[ServiceSDBus]sd_bus_process Get msg="
                << sd_bus_message_get_member(m);
        sd_bus_message_unref(m);
    }
    ServiceBase::initThread();
}

// bool ServiceSDBus::Register(const QString &path, const QString &interface)
//{
//     if (m_bus == nullptr) {
//         return false;
//     }
//     QFileInfo fileInfo(ServiceBase::libPath());
//     if (!QLibrary::isLibrary(fileInfo.absoluteFilePath()))
//         return false;

//    // TODO: 释放lib、dbusobject、ServiceBase、unregisterObject等
//    QLibrary *lib = new QLibrary(fileInfo.absoluteFilePath());
//    ServiceObject objFunc = ServiceObject(lib->resolve("ServiceObject"));
//    if (!objFunc) {
//        qWarning() << "failed to resolve the `ServiceObject` method: "<<
//        fileInfo.fileName() ; if (lib->isLoaded())
//            lib->unload();
//        lib->deleteLater();
//        return false;
//    }

//    void *obj = objFunc(ServiceBase::path().toStdString().c_str(),
//    int(ServiceBase::path().toStdString().length())); if (!obj) {
//        return false;
//    }
//    sd_bus_slot *slot = NULL;
//    if (sd_bus_add_object_vtable(m_bus, &slot,
//                                    ServiceBase::path().toStdString().c_str(),
//                                    ServiceBase::interface().toStdString().c_str(),
//                                    // TODO:interface问题 (const sd_bus_vtable
//                                    *)obj, NULL) < 0) {
//        qWarning() << "[hook]sd_bus_add_object_vtable error";
//        return -1;
//    }

//    ServiceBase::Register("", "");

//    return true;
//}

bool ServiceSDBus::registerService()
{
    if (m_bus == nullptr) {
        return false;
    }
    QFileInfo fileInfo(QString(SERVICE_LIB_DIR) + policy->libPath);
    if (!QLibrary::isLibrary(fileInfo.absoluteFilePath()))
        return false;

    // TODO: 释放lib、dbusobject、ServiceBase、unregisterObject等
    QLibrary *lib = new QLibrary(fileInfo.absoluteFilePath());
    DSMRegisterObject objFunc =
        DSMRegisterObject(lib->resolve("DSMRegisterObject"));
    if (!objFunc) {
        qWarning() << "[ServiceSDBus]failed to resolve the `DSMRegisterObject` "
                      "method: "
                   << fileInfo.fileName();
        if (lib->isLoaded())
            lib->unload();
        lib->deleteLater();
        return false;
    }

    int ret = objFunc(policy->name.toStdString().c_str(),
                      (void *)m_bus);  // TODO:old type cast
    if (ret) {
        return false;
    }

    ServiceBase::registerService();

    return true;
}
