#include "server.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug> // 用於在控制台輸出調試信息

Server::Server(QObject *parent)
    : QObject{parent}
{
    m_tcpServer = new QTcpServer(this);

    // 連接 newConnection 信號到我們的槽函數
    // 每當有新的客戶端連接進來，onNewConnection() 就會被調用
    connect(m_tcpServer, &QTcpServer::newConnection, this, &Server::onNewConnection);

    // 監聽本地所有 IP 地址的 12345 端口
    // 如果監聽失敗，打印錯誤信息
    if (!m_tcpServer->listen(QHostAddress::Any, 12345)) {
        qDebug() << "Server could not start!";
    } else {
        qDebug() << "Server started on port 12345. Waiting for connections...";
    }
}

void Server::onNewConnection()
{
    // nextPendingConnection() 返回下一個掛起的連接作為一個已連接的 QTcpSocket 物件
    QTcpSocket *clientSocket = m_tcpServer->nextPendingConnection();
    if (clientSocket)
    {
        qDebug() << "Client connected:" << clientSocket->peerAddress().toString();

        // 將新 socket 的信號連接到我們的槽
        connect(clientSocket, &QTcpSocket::readyRead, this, &Server::onReadyRead);
        connect(clientSocket, &QTcpSocket::disconnected, this, &Server::onDisconnected);
    }
}

void Server::onReadyRead()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (!clientSocket) return;

    // 简化的演示，暂时不处理粘包和分包的复杂情况
    // 假设一次能读到一个完整的包（包头+包体）
    // 在实际项目中，这里需要一个循环和缓冲区来处理
    QDataStream in(clientSocket);
    in.setVersion(QDataStream::Qt_6_0); // 确保版本一致

    if (clientSocket->bytesAvailable() < sizeof(qint32))
        return; // 数据不够，等下一次

    qint32 blockSize;
    in >> blockSize; // 读取包头

    if (clientSocket->bytesAvailable() < blockSize)
        return; // 数据不完整，等下一次

    //QByteArray jsonData;
    //QDataStream jsonStream(&jsonData, QIODevice::ReadOnly);
    //in >> jsonData; // 读取包体

    QByteArray jsonData = clientSocket->read(blockSize);
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);


    if(doc.isObject())
    {
        QJsonObject json = doc.object();
        QString type = json["type"].toString();
        qDebug() << "Received JSON of type:" << type;

        // 在这里根据不同的 type 来处理不同的逻辑
        if (type == "login") {
            handleLogin(clientSocket, json);
        } else if (type == "chat_message") {
            handleChatMessage(clientSocket, json);
        }
    }
}


// 處理登錄請求
void Server::handleLogin(QTcpSocket *socket, const QJsonObject &json)
{
    QString username = json["username"].toString();
    QString password = json["password"].toString();

    // 暫時硬編碼用戶驗證邏輯
    bool loginSuccess = (username == "userA" && password == "passA") ||
                        (username == "userB" && password == "passB");

    QJsonObject response;
    if (loginSuccess && !m_loggedUsers.contains(username)) {
        // 登錄成功
        m_clients[socket] = username;
        m_loggedUsers.insert(username);

        response["type"] = "login_success";
        response["username"] = username;
        //socket->write(QJsonDocument(response).toJson()); // 只回復給當前客戶端
        sendMessage(socket, response);
        qDebug() << "User" << username << "logged in.";

        // 向所有客戶端廣播更新後的用戶列表
        broadcastUserList();
    } else {
        // 登錄失敗
        response["type"] = "login_failure";
        response["reason"] = "錯誤的用戶名或密碼，或用戶已登錄。";
        //socket->write(QJsonDocument(response).toJson());
        sendMessage(socket, response);
    }
}

// 處理聊天消息轉發
void Server::handleChatMessage(QTcpSocket *socket, const QJsonObject &json)
{
    QString from = json["from"].toString();
    QString to = json["to"].toString();

    // 在 m_clients 中找到接收者 'to' 的 socket
    QTcpSocket *destSocket = nullptr;
    for (auto it = m_clients.begin(); it != m_clients.end(); ++it) {
        if (it.value() == to) {
            destSocket = it.key();
            break;
        }
    }

    // 如果找到了接收者，則將原消息轉發給他
    if (destSocket) {
        destSocket->write(QJsonDocument(json).toJson());
        sendMessage(destSocket,json);
    } else {
        // (可選) 處理用戶離線的情況，例如回覆發送者一條提示
        qDebug() << "User" << to << "not found or offline.";
    }
}

// 廣播用戶列表
void Server::broadcastUserList()
{
    QJsonObject userListMessage;
    userListMessage["type"] = "user_list_update";
    userListMessage["users"] = QJsonArray::fromStringList(m_loggedUsers.values());

    QByteArray data = QJsonDocument(userListMessage).toJson();
    // 向所有已連接的客戶端發送
    for (QTcpSocket *client : m_clients.keys()) {
        //client->write(data);
        sendMessage(client, userListMessage);
    }
}

void Server::onDisconnected()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (clientSocket)
    {
        QString username = m_clients.value(clientSocket);
        if (!username.isEmpty()) {
            m_clients.remove(clientSocket);
            m_loggedUsers.remove(username);
            qDebug() << "User" << username << "disconnected.";
            // 用戶下線，再次廣播用戶列表
            broadcastUserList();
        }
        clientSocket->deleteLater();
    }
}

void Server::sendMessage(QTcpSocket *socket, const QJsonObject &json)
{
    if (!socket || socket->state() != QAbstractSocket::ConnectedState) return;

    QByteArray data = QJsonDocument(json).toJson(QJsonDocument::Compact);
    QByteArray header;
    QDataStream stream(&header, QIODevice::WriteOnly);
    qint32 dataSize = data.size();
    stream << dataSize;

    socket->write(header);
    socket->write(data);
}
