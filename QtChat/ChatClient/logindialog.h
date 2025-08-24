#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QAbstractSocket>
#include <QListWidgetItem>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QTcpSocket *socket, QWidget *parent = nullptr);
    ~LoginDialog();
    QString getUsername() const; //獲取登入成功後的使用者名稱

private slots:
    void on_loginButton_clicked();
    void onConnected();
    void onErrorOccurred(QAbstractSocket::SocketError socketError);
    void onReadyRead();
    void on_registerButton_clicked();
    void on_showPasswordCheckBox_toggled(bool checked);

private:
    Ui::LoginDialog *ui;
    QTcpSocket *m_tcpSocket;
    QString m_username;

private:
    void sendMessage(const QJsonObject &json);
    void handleLoginSuccess(const QJsonObject &json);
    void handleLoginFailure(const QJsonObject &json);
    void handleRegistrationSuccess(const QJsonObject &json);
    void handleRegistrationFailure(const QJsonObject &json);
};

#endif // LOGINDIALOG_H
