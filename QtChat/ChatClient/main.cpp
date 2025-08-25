#include "mainwindow.h"
#include "logindialog.h"
#include "chatclienthandler.h"

#include <QApplication>
#include <QTcpSocket> // 引入標頭檔

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTcpSocket *socket = new QTcpSocket();
    ChatClientHandler *handler = new ChatClientHandler(socket);
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
        return 0; // 登入失敗或取消，程式退出
    }
}
