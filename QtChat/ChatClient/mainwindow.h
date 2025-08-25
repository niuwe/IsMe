#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAbstractSocket>
#include <QListWidgetItem>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "chatclienthandler.h"

QT_BEGIN_NAMESPACE
namespace Ui {class MainWindow;}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

private slots:
    void on_sendButton_clicked();
    // void onConnected();
    // void onErrorOccurred(QAbstractSocket::SocketError socketError);
    //void onReadyRead();
    void onJsonReceived(const QJsonObject &json);
    //void on_connectButton_clicked();
private:
    //void sendMessage(const QJsonObject &json);
    // void handleLoginSuccess(const QJsonObject &json);
    // void handleLoginFailure(const QJsonObject &json);
    void handleUserListUpdate(const QJsonObject &json);
    void handleChatMessage(const QJsonObject &json);

public:
    ~MainWindow();
    explicit MainWindow(ChatClientHandler *handler,
                        const QString &username, QWidget *parent = nullptr);

private:
    Ui::MainWindow *ui;
    ChatClientHandler *m_handler;
    QString m_username;
    //qint32 m_currentBlockSize;

};
#endif
