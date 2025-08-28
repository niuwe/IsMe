#ifndef SERVER_H
#define SERVER_H
#include "sslserver.h"
#include <QObject>
#include <QTcpServer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMap>
#include <QSet>
#include <QJsonArray>
#include <QStandardPaths>
#include <QDir>
#include <QSslSocket>
#include <QSslConfiguration>
#include <QSslKey>
#include <QSslCertificate>

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr);

private slots:
    void onNewConnection();
    void onReadyRead();
    void onDisconnected();

private:
    QSslConfiguration m_sslConfig;
    QMap<QTcpSocket*, QString> m_clients;
    QSet<QString> m_loggedUsers;
    QMap<QString, QTcpSocket*> m_usernameToSocketMap;
    QMap<QTcpSocket*, qint32> m_clientBlockSizes;
    QString m_userFilePath;
    QVariantMap m_userCredentials;
    //QMap<QString, QVariant> m_userCredentials;
    SslServer *m_tcpServer;

private:
    void handleLogin(QTcpSocket *socket, const QJsonObject &json);
    void handleChatMessage(QTcpSocket *socket, const QJsonObject &json);
    void sendMessage(QTcpSocket *socket, const QJsonObject &json);
    void broadcastUserList();
    void handleUserListRequest(QTcpSocket *socket);
    void handleRegistration(QTcpSocket *socket, const QJsonObject &json);
    void saveUsersToFile() const;
    void loadUsersFromFile();

};

#endif // SERVER_H
