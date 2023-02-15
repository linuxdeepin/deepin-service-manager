#include "serviceqtdbus.h"

#include "policy/policy.h"
#include "qtdbushook.h"

#include <QDBusAbstractAdaptor>
#include <QDebug>
#include <QFileInfo>
#include <QLibrary>
#include <QMetaClassInfo>
#include <QThread>

ServiceQtDBus::ServiceQtDBus(QObject *parent)
    : ServiceBase(parent)
{
    m_SDKType = SDKType::QT;
}

QDBusConnection ServiceQtDBus::qDbusConnection()
{
    if (policy->name.isEmpty()) {
        if (m_sessionType == QDBusConnection::SystemBus) {
            return QDBusConnection::systemBus();
        } else {
            return QDBusConnection::sessionBus();
        }
    } else {
        if (m_sessionType == QDBusConnection::SystemBus) {
            return QDBusConnection::connectToBus(QDBusConnection::SystemBus, policy->name);
        } else {
            return QDBusConnection::connectToBus(QDBusConnection::SessionBus, policy->name);
        }
    }
}

void ServiceQtDBus::initThread()
{
    qInfo() << "[ServiceQtDBus]init service: " << policy->paths();
    qDbusConnection().registerService(policy->name);
    qDbusConnection().registerObject(QStringLiteral("/PrivateDeclaration"), this);

    // TODO:无权限、隐藏、按需启动需求的service，不应该注册，避免触发hook，提高效率
    QTDbusHook::instance()->setServiceObject(this);

    if (policy->isResident()) {
        registerService();
    }
    ServiceBase::initThread();
}

// bool ServiceQtDBus::Register(const QString &path, const QString &interface)
//{
//     qInfo() << "ServiceManager::Register::" << path;

//    QFileInfo fileInfo(ServiceBase::libPath());
//    if (!QLibrary::isLibrary(fileInfo.absoluteFilePath()))
//        return false;

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
//    int(ServiceBase::path().toStdString().length()));

//    QObject *qobj = static_cast<QObject*>(obj); // TODO 异常处理
//    if (!qobj) {
//        return false;
//    }
//    QDBusConnection::RegisterOptions opts = QDBusConnection::ExportAllSlots |
//    QDBusConnection::ExportAllSignals | QDBusConnection::ExportAllProperties;
//    const QObjectList &children = qobj->children();
//    QObjectList::ConstIterator it = children.constBegin();
//    QObjectList::ConstIterator end = children.constEnd();
//    for ( ; it != end; ++it) {
//        QDBusAbstractAdaptor *adaptor =
//        qobject_cast<QDBusAbstractAdaptor*>(*it); if (adaptor) {
//            opts = QDBusConnection::ExportAdaptors;
//            break;
//        }
//    }
////    Service *s = new Service();
////    new dbusdemoAdaptor(s);
//    int idx = qobj->metaObject()->indexOfClassInfo("D-Bus Interface");
//    qInfo() << "D-Bus Interface" <<
//    QString::fromUtf8(qobj->metaObject()->classInfo(idx).value());
//    qDbusConnection().registerObject(ServiceBase::path(), qobj, opts); // TODO
//    path

//    ServiceBase::Register("", "");
//    return true;
//}

bool ServiceQtDBus::registerService()
{
    qInfo() << "[ServiceQtDBus]service register: " << policy->name;

    QFileInfo fileInfo(QString(SERVICE_LIB_DIR) + policy->libPath);
    if (!QLibrary::isLibrary(fileInfo.absoluteFilePath()))
        return false;

    // TODO: 释放lib、dbusobject、ServiceBase、unregisterObject等
    QLibrary *lib = new QLibrary(fileInfo.absoluteFilePath());
    DSMRegister objFunc = DSMRegister(lib->resolve("DSMRegister"));
    if (!objFunc) {
        qWarning() << "[ServiceQtDBus]failed to resolve the "
                      "`DSMRegister` method: "
                   << fileInfo.fileName();
        if (lib->isLoaded())
            lib->unload();
        lib->deleteLater();
        return false;
    }
    auto connection = qDbusConnection();
    int ret = objFunc(policy->name.toStdString().c_str(), (void *)&connection);
    if (ret) {
        return false;
    }

    ServiceBase::registerService();
    return true;
}
