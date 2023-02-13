#pragma once

#include <QCoreApplication>
#include <QDBusConnection>
#include <QDebug>

class Service1 : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.services.demo")
public:
    explicit Service1(QObject *parent = 0)
        : QObject(parent)
    {
    }

public slots:

    void SetData(QString data)
    {
        qInfo() << "Data:" << data;
        // dosomething
    }
};