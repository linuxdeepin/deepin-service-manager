#include "demoadaptor.h"
#include "service.h"

#include <QApplication>
#include <QDBusConnection>
#include <QDebug>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QWidget *widget = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout();
    widget->setLayout(mainLayout);
    QTextEdit *textEdit = new QTextEdit();
    mainLayout->addWidget(textEdit);
    widget->setMinimumSize(800, 600);
    widget->show();

    Service s;
    DemoAdaptor adp(&s);
    if (!QDBusConnection::sessionBus().registerObject("/org/deepin/service/sdk/demo", &s)) {
        qWarning() << "failed to register dbus object"
                   << QDBusConnection::sessionBus().lastError().message();
    }
    if (!QDBusConnection::sessionBus().registerService("org.deepin.service.sdk.demo")) {
        qWarning() << "failed to register dbus object"
                   << QDBusConnection::sessionBus().lastError().message();
    }

    QObject::connect(&s, &Service::dbusLog, textEdit, &QTextEdit::append);

    return a.exec();
}
