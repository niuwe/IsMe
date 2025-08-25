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

    // 先保存兩個用戶測試使用
    m_userCredentials["userA"] = "123";
    m_userCredentials["userB"] = "123";
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
            qDebug() << "Received chat_message"<<json ;
            handleChatMessage(clientSocket, json);
        } else if (type == "request_user_list") { // <-- 新增的處理分支
            handleUserListRequest(clientSocket);
        } else if (type == "register") { // <-- 新增分支
            handleRegistration(clientSocket, json);
        }
    }
}


// 處理登錄請求
void Server::handleLogin(QTcpSocket *socket, const QJsonObject &json)
{
    QString username = json["username"].toString();
    QString password = json["password"].toString();

    // 暫時硬編碼用戶驗證邏輯
    // bool loginSuccess = (username == "userA" && password == "passA") ||
    //                     (username == "userB" && password == "passB") ||
    //                     (username == "userC" && password == "passC");

    bool loginSuccess = m_userCredentials.contains(username) &&
                        (m_userCredentials.value(username) == password);

    QJsonObject response;
    if (loginSuccess && !m_clients.contains(socket)) {

        m_clients[socket] = username;
        m_loggedUsers.insert(username);
        m_usernameToSocketMap[username] = socket;

        response["type"] = "login_success";
        response["username"] = username;
        sendMessage(socket, response); // 只發送登入成功訊息
        qDebug() << "User" << username << "logged in.";

        // 向【其他】所有客戶端廣播更新後的用戶列表
        // 注意：這裡依然可以廣播，但剛登入的客戶端會自己請求一次，確保能收到。
        // 或者為了簡化，可以把廣播完全移到用戶離線時。
        // 這裡我們先保留廣播，讓其他用戶能即時看到新人上線。
        broadcastUserList();
    } else {
        // 登錄失敗
        response["type"] = "login_failure";
        response["reason"] = "錯誤的用戶名或密碼，或用戶已登錄。";
        sendMessage(socket, response);
        qDebug() << "reponse: " << response;
    }
}

// 處理聊天消息轉發
void Server::handleChatMessage(QTcpSocket *socket, const QJsonObject &json)
{
    QString to = json["to"].toString();

    if (!m_clients.contains(socket)) {
        qDebug() << "Warning: Received message from an unauthenticated socket.";
        return;
    }
    QString from = m_clients.value(socket);
    QJsonObject messageToSend = json;
    messageToSend["from"] = from;

    // O(log N) 查找，高效且简洁
    if (m_usernameToSocketMap.contains(to)) {
        QTcpSocket *destSocket = m_usernameToSocketMap.value(to);
        sendMessage(destSocket, messageToSend);
    } else {
        qDebug() << "User" << to << "not found or offline.";
        // (可选) 可以向发送者回发一条“用户不在线”的提示
    }
}

// 廣播用戶列表
void Server::broadcastUserList()
{
    QJsonObject userListMessage;
    userListMessage["type"] = "user_list_update";
    userListMessage["users"] = QJsonArray::fromStringList(m_loggedUsers.values());

    // 使用 constBegin() 和 constEnd()，我们不打算在循环中修改 map。
    for (auto it = m_clients.constBegin(); it != m_clients.constEnd(); ++it) {
        // it.key() 就是我们需要的 QTcpSocket*
        sendMessage(it.key(), userListMessage);
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
            m_usernameToSocketMap.remove(username);
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

    qDebug() << "sended message is:" << json;
    socket->write(header);
    socket->write(data);
}

void Server::handleUserListRequest(QTcpSocket *socket)
{
    QJsonObject userListMessage;
    userListMessage["type"] = "user_list_update";
    // QSet to QList, then to QJsonArray
    userListMessage["users"] = QJsonArray::fromStringList(m_loggedUsers.values());

    // 只向請求的客戶端發送完整的用戶列表
    sendMessage(socket, userListMessage);
    qDebug() << "Sent user list to" << m_clients.value(socket);
}

// server.cpp
void Server::handleRegistration(QTcpSocket *socket, const QJsonObject &json)
{
    QString username = json["username"].toString();
    QString password = json["password"].toString();

    QJsonObject response;

    // 審核邏輯：檢查 m_userCredentials 中是否已存在該使用者名稱
    if (m_userCredentials.contains(username)) {
        // 使用者名稱已被佔用
        response["type"] = "registration_failure";
        response["reason"] = "此使用者名稱已被註冊，請更換一個。";
        qDebug() << "Registration failed for" << username << ": Username taken.";
    } else {
        // 註冊成功，將新帳號密碼存入 QMap
        m_userCredentials[username] = password;
        response["type"] = "registration_success";
        qDebug() << "User" << username << "registered successfully.";
        // (可選) 在這裡將帳號密碼寫入檔案或資料庫，以便伺服器重啟後不清空
    }

    sendMessage(socket, response);
}
