#include "service/serviceqtdbus.h"
#include "service/qtdbushook.h"

#include <QDebug>
#include <QThread>
#include <QLibrary>
#include <QFileInfo>
#include <QMetaClassInfo>
#include <QDBusAbstractAdaptor>


ServiceQtDBus::ServiceQtDBus(QObject *parent) : ServiceBase(parent)
{
    m_SDKType = SDKType::QT;
}

QDBusConnection ServiceQtDBus::qDbusConnection()
{
    if (m_policy->m_name.isEmpty()) { // TODO
        if (m_sessionType == SessionType::System) {
            // qInfo() << "qDbusConnection::system::default";
            return QDBusConnection::systemBus();
        } else {
            // qInfo() << "qDbusConnection::session::default";
            return QDBusConnection::sessionBus();
        }
    } else {
        if (m_sessionType == SessionType::System) {
            // qInfo() << "qDbusConnection::system::" << m_pluginConfig->m_dbusName;
            return QDBusConnection::connectToBus(QDBusConnection::SystemBus, m_policy->m_name);
        } else {
            // qInfo() << "qDbusConnection::session::" << m_pluginConfig->m_dbusName;
            return QDBusConnection::connectToBus(QDBusConnection::SessionBus, m_policy->m_name);
        }
    }
}

void ServiceQtDBus::InitService()
{
    qInfo() << "[ServiceQtDBus]init service:" << libPath() << ", InitService-ThreadID:" << QThread::currentThreadId();

    QThread *th = new QThread();
    moveToThread(th);
    QObject::connect(th, SIGNAL(started()), this, SLOT(InitThread()));
    th->start();
    // InitThread();
}

void ServiceQtDBus::InitThread()
{
    qInfo() << "[ServiceQtDBus]init service" << ServiceBase::paths() << ", InitThread-ThreadID:" << QThread::currentThreadId();
    qDbusConnection().registerService(ServiceBase::name());
    qDbusConnection().registerObject(QStringLiteral("/PrivateDeclaration"), this); // TODO:this

    // TODO:无权限、隐藏、按需启动需求的service，不应该注册，避免触发hook，提高效率
    QTDbusHook::instance()->setServiceObject(this);

//    m_policy = new Policy(this);
//    m_policy->ParseConfig(policyPath());
//    m_policy->Print(); // TODO

    if (isResident()) {
        Register();
    }
}

//bool ServiceQtDBus::Register(const QString &path, const QString &interface)
//{
//    qInfo() << "ServiceManager::Register::" << path;

//    QFileInfo fileInfo(ServiceBase::libPath());
//    if (!QLibrary::isLibrary(fileInfo.absoluteFilePath()))
//        return false;

//    // TODO: 释放lib、dbusobject、ServiceBase、unregisterObject等
//    QLibrary *lib = new QLibrary(fileInfo.absoluteFilePath());
//    ServiceObject objFunc = ServiceObject(lib->resolve("ServiceObject"));
//    if (!objFunc) {
//        qWarning() << "failed to resolve the `ServiceObject` method: "<< fileInfo.fileName() ;
//        if (lib->isLoaded())
//            lib->unload();
//        lib->deleteLater();
//        return false;
//    }

//    void *obj = objFunc(ServiceBase::path().toStdString().c_str(), int(ServiceBase::path().toStdString().length()));

//    QObject *qobj = static_cast<QObject*>(obj); // TODO 异常处理
//    if (!qobj) {
//        return false;
//    }
//    QDBusConnection::RegisterOptions opts = QDBusConnection::ExportAllSlots | QDBusConnection::ExportAllSignals | QDBusConnection::ExportAllProperties;
//    const QObjectList &children = qobj->children();
//    QObjectList::ConstIterator it = children.constBegin();
//    QObjectList::ConstIterator end = children.constEnd();
//    for ( ; it != end; ++it) {
//        QDBusAbstractAdaptor *adaptor = qobject_cast<QDBusAbstractAdaptor*>(*it);
//        if (adaptor) {
//            opts = QDBusConnection::ExportAdaptors;
//            break;
//        }
//    }
////    Service *s = new Service();
////    new dbusdemoAdaptor(s);
//    int idx = qobj->metaObject()->indexOfClassInfo("D-Bus Interface");
//    qInfo() << "D-Bus Interface" << QString::fromUtf8(qobj->metaObject()->classInfo(idx).value());
//    qDbusConnection().registerObject(ServiceBase::path(), qobj, opts); // TODO path

//    ServiceBase::Register("", "");
//    return true;
//}

bool ServiceQtDBus::Register()
{
    qInfo() << "[ServiceQtDBus]service register:" << ServiceBase::libPath();

    QFileInfo fileInfo(QString(DEEPIN_SERVICE_MANAGER_DIR)+ServiceBase::libPath());
    if (!QLibrary::isLibrary(fileInfo.absoluteFilePath()))
        return false;

    // TODO: 释放lib、dbusobject、ServiceBase、unregisterObject等
    QLibrary *lib = new QLibrary(fileInfo.absoluteFilePath());
    DSMRegisterObject objFunc = DSMRegisterObject(lib->resolve("DSMRegisterObject"));
    if (!objFunc) {
        qWarning() << "failed to resolve the `DSMRegisterObject` method: "<< fileInfo.fileName() ;
        if (lib->isLoaded())
            lib->unload();
        lib->deleteLater();
        return false;
    }
    int ret = objFunc(ServiceBase::name().toStdString().c_str(), nullptr); // TODO
    if (ret) {
        return false;
    }

    ServiceBase::Register();
    return true;
}
