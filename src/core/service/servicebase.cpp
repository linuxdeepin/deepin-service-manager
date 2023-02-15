#include "servicebase.h"

#include "policy/policy.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDebug>
#include <QThread>

ServiceBase::ServiceBase(QObject *parent)
    : QObject(parent)
    , policy(nullptr)
    , m_isRegister(false)
{
}

ServiceBase::~ServiceBase() { }

void ServiceBase::init(const QDBusConnection::BusType &busType, Policy *p)
{
    m_sessionType = busType;
    policy = p;
    p->setParent(this);
    // p->Print();

    initService();
}

void ServiceBase::initService()
{
    QThread *th = new QThread();
    setParent(nullptr);
    moveToThread(th);
    connect(th, &QThread::started, this, &ServiceBase::initThread);
    th->start();
}

void ServiceBase::initThread() { }

bool ServiceBase::isRegister() const
{
    return m_isRegister;
}

bool ServiceBase::registerService()
{
    m_isRegister = true;
    return true;
}
