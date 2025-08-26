// client/main.cpp - 修改后
#include "mainwindow.h"
#include "logindialog.h"
#include "chatclienthandler.h"
#include <QApplication>
#include <QTcpSocket>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTcpSocket *socket = new QTcpSocket(&a);
    ChatClientHandler *handler = new ChatClientHandler(socket, &a);


    handler->connectToServer("127.0.0.1", 12345);

    LoginDialog *loginDialog = new LoginDialog(handler);

    if (loginDialog->exec() == QDialog::Accepted)
    {
        QString username = loginDialog->getUsername();
        MainWindow w(handler, username);
        w.show();
        return a.exec();
    }
    else
    {
        return 0;
    }
}
