#include "server.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QFile>
#include <QCryptographicHash>
#include <QUuid>
#include "sslserver.h"

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

    // --- SSL configuration starts ---
    QFile certFile(":/ssl/server.crt");
    QFile keyFile(":/ssl/server.key");
    if (!certFile.open(QIODevice::ReadOnly) || !keyFile.open(QIODevice::ReadOnly)) {
        qFatal("Could not open certificate or key file for SSL!");
        return;
    }
    QSslCertificate certificate(&certFile);
    QByteArray keyData = keyFile.readAll();
    // The constructor will automatically detect the PEM format,
    // with no password by default.
    QSslKey key(keyData, QSsl::Rsa);
    certFile.close();
    keyFile.close();

    m_sslConfig = QSslConfiguration::defaultConfiguration();
    m_sslConfig.setLocalCertificate(certificate);
    m_sslConfig.setPrivateKey(key);
    m_sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone); // 服务端不需要验证客户端证书
    // --- SSL Configuration completed ---

    m_tcpServer = new SslServer(this);
    m_tcpServer->setSslConfiguration(m_sslConfig);

    // Whenever a new client connects, onNewConnection() will be called
    connect(m_tcpServer, &SslServer::newConnection, this, &Server::onNewConnection);

    // Listen to ports 12345 of all local IP addresses
    if (!m_tcpServer->listen(QHostAddress::Any, 12345)) {
        qDebug() << "Server could not start!";
    } else {
        qDebug() << "Server started on port 12345. Waiting for connections...";
    }
}


void Server::onNewConnection()
{
    while (m_tcpServer->hasPendingConnections()) {
        // now extPendingConnection return  one QSslSocket object
        QSslSocket *sslSocket = qobject_cast<QSslSocket*>(m_tcpServer->nextPendingConnection());
        if (sslSocket) {
            // Before connecting to Signal, enable server-side encryption
            connect(sslSocket, &QSslSocket::encrypted, this, [this, sslSocket](){
                qDebug() << "Connection is encrypted. Client:" << sslSocket->peerAddress().toString();
                // Only after encryption is successful, connect to other signals
                connect(sslSocket, &QTcpSocket::readyRead, this, &Server::onReadyRead);
                connect(sslSocket, &QTcpSocket::disconnected, this, &Server::onDisconnected);
            });
            //  SSL handshake error
            connect(sslSocket, &QSslSocket::sslErrors, this, [](const QList<QSslError> &errors){
                qWarning() << "SSL Errors:" << errors;
            });
        }
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
    QJsonObject response;

    // --- Hash verification ---
    if (m_userCredentials.contains(username)) {
        QVariantMap userData = m_userCredentials.value(username).toMap();
        QString salt = userData["salt"].toString();
        QByteArray storedHash = QByteArray::fromHex(userData["hash"].toByteArray()); // 从16进制字符串恢复

        // Use the same salt to hash the password submitted by the user
        QByteArray calculatedHash = QCryptographicHash::hash((password + salt).toUtf8(), QCryptographicHash::Sha256);

        // Compare hash values ​​to see if they are consistent
        if (calculatedHash == storedHash) {
            // Hash matches, verification successful
            if (m_loggedUsers.contains(username)) {
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
        } else {
            // Hash mismatch, wrong password
            response["type"] = "login_failure";
            response["reason"] = "Incorrect password.";
            sendMessage(socket, response);
            qDebug() << "Login failed for" << username << ": Invalid credentials.";
        }
    } else {
        // Username does not exist
        response["type"] = "login_failure";
        response["reason"] = "Username does not exist.";
        sendMessage(socket, response);
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
        // --- 核心修改：哈希处理 ---
        // 1. 生成一个随机的盐 (salt)
        QString salt = QUuid::createUuid().toString();
        // 2. 将密码和盐组合，进行SHA-256哈希
        QByteArray hash = QCryptographicHash::hash((password + salt).toUtf8(), QCryptographicHash::Sha256);

        // 3. 将用户名、哈希值(Hex格式)、盐存入 QMap
        // m_userCredentials 现在需要存储更复杂的数据，我们使用 QVariantMap
        QVariantMap userData;
        userData["hash"] = hash.toHex(); // 转换为16进制字符串存储
        userData["salt"] = salt;
        m_userCredentials[username] = userData;
        // --- 修改结束 ---

        //m_userCredentials[username] = password;
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

    // QJsonObject usersObject;
    // for (auto it = m_userCredentials.constBegin(); it != m_userCredentials.constEnd(); ++it) {
    //     usersObject.insert(it.key(), it.value());
    // }

    // 直接将 QVariantMap 转换为 QJsonObject
    QJsonObject usersObject = QJsonObject::fromVariantMap(m_userCredentials);

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
    // 遍历JSON对象，将其转换为QVariantMap
    for (auto it = usersObject.constBegin(); it != usersObject.constEnd(); ++it) {
        // 将每个用户的数据(也是一个QJsonObject)转换为QVariantMap
        m_userCredentials.insert(it.key(), it.value().toObject().toVariantMap());
    }
    qDebug() << "Loaded" << m_userCredentials.size() << "users from file.";
    // for (auto it = usersObject.constBegin(); it != usersObject.constEnd(); ++it) {
    //     if (it.value().isString()) {
    //         m_userCredentials.insert(it.key(), it.value().toString());
    //     }
    // }
    // qDebug() << "Loaded" << m_userCredentials.size() << "users from file.";
}
