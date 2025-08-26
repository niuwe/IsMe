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
<<<<<<< HEAD
    if (loginDialog->exec() == QDialog::Accepted)
    {
        QString username = loginDialog->getUsername();
        delete loginDialog;
=======

    if (loginDialog->exec() == QDialog::Accepted)
    {
        QString username = loginDialog->getUsername();
>>>>>>> 72264e37dc7391754473f9c84667e8bc95cccd56
        MainWindow w(handler, username);
        w.show();
        return a.exec();
    }
    else
    {
        delete loginDialog;
        return 0;
    }
}
