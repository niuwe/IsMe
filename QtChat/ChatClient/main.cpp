#include "mainwindow.h"
#include "logindialog.h"
#include <QApplication>
#include <QTcpSocket> // 引入標頭檔

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 1. 在 main 函式中建立 QTcpSocket 物件
    // 它的生命週期將貫穿整個應用程式
    QTcpSocket socket ;


    // 2. 建立登入對話方塊，並將 socket 的指標傳遞進去
    LoginDialog loginDialog(&socket);

    // 3. 執行登入對話方塊
    if (loginDialog.exec() == QDialog::Accepted)
    {
        // 4. 登入成功，獲取使用者名稱
       QString username = loginDialog.getUsername();

        // 5. 建立主視窗，並將同一個 socket 的指標和使用者名稱傳遞進去
        MainWindow w(&socket, username);
        w.show();

        return a.exec();
    }
    else
    {
        return 0; // 登入失敗或取消，程式退出
    }
}
