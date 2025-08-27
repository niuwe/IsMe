#include "server.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QFile>

Server::Server(QObject *parent)
    : QObject{parent}
{
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dataPath);
    if (!dir.exists())
    {
        dir.mkpath(".");
    }
    m_userFilePath = dataPath + "/users.json";
    qDebug() << "User data will be stored at:" << m_userFilePath;
    loadUsersFromFile();

    m_tcpServer = new QTcpServer(this);
    // Whenever a new client connects, onNewConnection() will be called
    connect(m_tcpServer, &QTcpServer::newConnection, this, &Server::onNewConnection);

    // Listen to ports 12345 of all local IP addresses
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

        connect(clientSocket, &QTcpSocket::readyRead, this, &Server::onReadyRead);
        connect(clientSocket, &QTcpSocket::disconnected, this, &Server::onDisconnected);
    }
}

void Server::onReadyRead()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (!clientSocket) return;

    QDataStream in(clientSocket);
    in.setVersion(QDataStream::Qt_6_0);

    qint32 &blockSize = m_clientBlockSizes[clientSocket];

    while (true)
    {

        if (blockSize == 0) {

            if (clientSocket->bytesAvailable() < sizeof(qint32)) {
                break;
            }

            in >> blockSize;
        }

        if (clientSocket->bytesAvailable() < blockSize) {
            break;
        }

        QByteArray jsonData = clientSocket->read(blockSize);

        blockSize = 0;

        QJsonDocument doc = QJsonDocument::fromJson(jsonData);
        if(doc.isObject())
        {
            QJsonObject json = doc.object();
            QString type = json["type"].toString();
            qDebug() << "Received JSON of type:" << type;

            if (type == "login") {
                handleLogin(clientSocket, json);
            } else if (type == "chat_message") {
                qDebug() << "Received chat_message"<<json ;
                handleChatMessage(clientSocket, json);
            } else if (type == "request_user_list") {
                handleUserListRequest(clientSocket);
            } else if (type == "register") {
                handleRegistration(clientSocket, json);
            }
        }

        if (clientSocket->bytesAvailable() == 0)
        {
            break;
        }
    }
}


void Server::handleLogin(QTcpSocket *socket, const QJsonObject &json)
{
    QString username = json["username"].toString();
    QString password = json["password"].toString();

    bool credentialsValid  = m_userCredentials.contains(username) &&
                        (m_userCredentials.value(username) == password);

    QJsonObject response;
    // Incorrect username or password.
    if(!credentialsValid)
    {
        response["type"] = "login_failure";
        response["reason"] = "Incorrect username or password.";
        sendMessage(socket, response);
        qDebug() << "Login failed for" << username << ": Invalid credentials.";
    }
    // user is already logged in
    else if (m_loggedUsers.contains(username))
    {
        response["type"] = "login_failure";
        response["reason"] = "User is already logged in.";
        sendMessage(socket, response);
        qDebug() << "reponse: " << response;

    } else {
        m_clients[socket] = username;
        m_loggedUsers.insert(username);
        m_usernameToSocketMap[username] = socket;

        response["type"] = "login_success";
        response["username"] = username;
        sendMessage(socket, response);
        qDebug() << "User" << username << "logged in.";
        broadcastUserList();
    }
}

// Handling chat message
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

    // O(log N)
    if (m_usernameToSocketMap.contains(to)) {
        QTcpSocket *destSocket = m_usernameToSocketMap.value(to);
        sendMessage(destSocket, messageToSend);
    } else {
        qDebug() << "User" << to << "not found or offline.";
        QJsonObject response;
        response["type"] = "message_failure";
        response["reason"] = QString("%1 is already offline").arg(to);
        sendMessage(socket, response);
    }
}

// broadcast user list
void Server::broadcastUserList()
{
    QJsonObject userListMessage;
    userListMessage["type"] = "user_list_update";
    userListMessage["users"] = QJsonArray::fromStringList(m_loggedUsers.values());

    for (auto it = m_clients.constBegin(); it != m_clients.constEnd(); ++it) {
        // it.key() is QTcpSocket* that we are need
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
            // The user goes offline and broadcasts the user list again
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
    // Only send the full user list to the requesting client
    sendMessage(socket, userListMessage);
}

void Server::handleRegistration(QTcpSocket *socket, const QJsonObject &json)
{
    QString username = json["username"].toString();
    QString password = json["password"].toString();

    QJsonObject response;

    if (m_userCredentials.contains(username)) {
        // Username is already taken
        response["type"] = "registration_failure";
        response["reason"] = "This username has already been registered."
                             " Please change it.";
        qDebug() << "Registration failed for" << username << ": Username taken.";
    } else {
        // Registration is successful, save the new account and password
        m_userCredentials[username] = password;
        response["type"] = "registration_success";
        qDebug() << "User" << username << "registered successfully.";
        saveUsersToFile();
    }

    sendMessage(socket, response);
}

void Server::saveUsersToFile() const
{
    QFile file(m_userFilePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Could not open users file for writing:" << file.errorString();
        return;
    }

    QJsonObject usersObject;
    for (auto it = m_userCredentials.constBegin(); it != m_userCredentials.constEnd(); ++it) {
        usersObject.insert(it.key(), it.value());
    }

    file.write(QJsonDocument(usersObject).toJson());
    file.close();
}

void Server::loadUsersFromFile()
{
    QFile file(m_userFilePath);
    if (!file.exists()) {
        qDebug() << "Users file does not exist. Starting with an empty user list.";
        return;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open users file for reading:" << file.errorString();
        return;
    }

    QByteArray fileData = file.readAll();
    file.close();

    QJsonObject usersObject = QJsonDocument::fromJson(fileData).object();

    // clear the user credentials and prepare to load form a file
    m_userCredentials.clear();

    for (auto it = usersObject.constBegin(); it != usersObject.constEnd(); ++it) {
        if (it.value().isString()) {
            m_userCredentials.insert(it.key(), it.value().toString());
        }
    }
    qDebug() << "Loaded" << m_userCredentials.size() << "users from file.";
}
