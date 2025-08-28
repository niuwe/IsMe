#include "chatclienthandler.h"
#include <QDataStream>
#include <QJsonDocument>
#include <QSslError>

ChatClientHandler::ChatClientHandler(QObject *parent)
    : QObject(parent)
    , m_currentBlockSize(0)
{
    m_tcpSocket = new QSslSocket(this);
    connect(m_tcpSocket, &QTcpSocket::readyRead, this, &ChatClientHandler::onReadyRead);
    connect(m_tcpSocket, &QTcpSocket::connected, this, &ChatClientHandler::connected);
    connect(m_tcpSocket, &QTcpSocket::errorOccurred, this, &ChatClientHandler::onErrorOccurred);
}

void ChatClientHandler::connectToServer(const QString &host, quint16 port)
{
    if (m_tcpSocket->state() == QAbstractSocket::UnconnectedState) {

        connect(m_tcpSocket, qOverload<const QList<QSslError> &>(&QSslSocket::sslErrors),
                m_tcpSocket, qOverload<const QList<QSslError> &>(&QSslSocket::ignoreSslErrors));

        // Use encrypted connection
        m_tcpSocket->connectToHostEncrypted(host, port);
    }
}

bool ChatClientHandler::isConnected() const
{
    return m_tcpSocket->state() == QAbstractSocket::ConnectedState;
}


void ChatClientHandler::onReadyRead()
{
    QDataStream in(m_tcpSocket);
    in.setVersion(QDataStream::Qt_6_0);
    while (true)
    {
        if (m_currentBlockSize == 0) {
            if (m_tcpSocket->bytesAvailable() < sizeof(qint32)) {
                return;
            }
            in >> m_currentBlockSize;
        }
        if (m_tcpSocket->bytesAvailable() < m_currentBlockSize) {
            return;
        }
        QByteArray jsonData = m_tcpSocket->read(m_currentBlockSize);
        m_currentBlockSize = 0;

        QJsonDocument doc = QJsonDocument::fromJson(jsonData);
        if(doc.isObject())
        {
            emit jsonMessageReceived(doc.object());
        }
        if (m_tcpSocket->bytesAvailable() == 0) {
            break;
        }
    }
}

void ChatClientHandler::sendMessage(const QJsonObject &json)
{
    if (m_tcpSocket->state() != QAbstractSocket::ConnectedState) return;
    QByteArray data = QJsonDocument(json).toJson(QJsonDocument::Compact);
    QDataStream out(m_tcpSocket);
    out.setVersion(QDataStream::Qt_6_0);
    out << (qint32)data.size(); //send head
    m_tcpSocket->write(data);   //send bady
}

void ChatClientHandler::onErrorOccurred(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    emit errorOccurred(m_tcpSocket->errorString());
}
