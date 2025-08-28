// client/main.cpp - 修改后
#include "mainwindow.h"
#include "logindialog.h"
#include "chatclienthandler.h"
#include <QApplication>
#include <QTcpSocket>
#include <QSslSocket>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ChatClientHandler *handler = new ChatClientHandler(&a);

    // handler->connectToServer("127.0.0.1", 12345);

    LoginDialog loginDialog(handler);
    if (loginDialog.exec() == QDialog::Accepted)
    {
        QString username = loginDialog.getUsername();
        MainWindow w(handler, username);
        w.show();
        return a.exec();
    }
    else
    {
        return 0;
    }
}
