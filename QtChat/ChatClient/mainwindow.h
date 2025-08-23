#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAbstractSocket>
#include <QListWidgetItem>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

QT_BEGIN_NAMESPACE
namespace Ui {class MainWindow;}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

private slots:
    // UI 按鈕的槽函數 (可以在 UI 設計器中右鍵點擊按鈕 -> Go to slot... 自動生成)
    void on_sendButton_clicked();
    void on_loginButton_clicked();
    // Socket 的槽函數
    void onConnected();
    void onErrorOccurred(QAbstractSocket::SocketError socketError);
    void onReadyRead();
    void on_connectButton_clicked();
private:
    void sendMessage(const QJsonObject &json);
    void handleLoginSuccess(const QJsonObject &json);
    void handleLoginFailure(const QJsonObject &json);
    void handleUserListUpdate(const QJsonObject &json);
    void handleChatMessage(const QJsonObject &json);

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QTcpSocket *m_tcpSocket;
    QString m_username;

};
#endif // MAINWINDOW_H
