#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QAbstractSocket>
#include <QListWidgetItem>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "chatclienthandler.h"

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(ChatClientHandler *handler, QWidget *parent = nullptr);
    ~LoginDialog();
    QString getUsername() const;

private slots:
    void on_loginButton_clicked();
    void on_registerButton_clicked();
    void on_showPasswordCheckBox_toggled(bool checked);
    void onConnected();
    void onJsonReceived(const QJsonObject &json);
    void onError(const QString &errorString);

private:
    Ui::LoginDialog *ui;
    ChatClientHandler *m_handler;
    QString m_username;
    QJsonObject m_pendingMessage;
};

#endif // LOGINDIALOG_H
