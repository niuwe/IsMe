#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMap>
#include <QSet>
#include <QJsonArray>

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr);

private slots:
    void onNewConnection();    // 處理新的客戶端連接
    void onReadyRead();        // 處理收到的數據
    void onDisconnected();     // 處理客戶端斷開連接

private:
    QTcpServer *m_tcpServer;
    QMap<QTcpSocket*, QString> m_clients;
    QSet<QString> m_loggedUsers;
    QMap<QString, QString> m_userCredentials;
    QMap<QString, QTcpSocket*> m_usernameToSocketMap;
    QMap<QTcpSocket*, qint32> m_clientBlockSizes;

private:
    void handleLogin(QTcpSocket *socket, const QJsonObject &json);
    void handleChatMessage(QTcpSocket *socket, const QJsonObject &json);
    void sendMessage(QTcpSocket *socket, const QJsonObject &json);
    void broadcastUserList();
    void handleUserListRequest(QTcpSocket *socket);
    void handleRegistration(QTcpSocket *socket, const QJsonObject &json);

};

#endif // SERVER_H
