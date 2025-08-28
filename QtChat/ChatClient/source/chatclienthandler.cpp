#include "chatclienthandler.h"
#include <QDataStream>
#include <QJsonDocument>
#include <QSslError>
#include <QFile>

ChatClientHandler::ChatClientHandler(QObject *parent)
    : QObject(parent)
    , m_currentBlockSize(0)
{
    // Loading our self-signed server certificate from a resource
    QFile certFile(":/rootCA.crt");
    if (!certFile.open(QIODevice::ReadOnly)) {
        qWarning() << "!!!!!! FATAL: Could not open certificate file from resources. Path ':/ssl/server.crt' is incorrect or resource file is missing.";
        return;
    }
    QSslCertificate certificate(&certFile, QSsl::Pem);
    certFile.close();

    if (certificate.isNull()) {
        qWarning() << "!!!!!! FATAL: The loaded certificate data is invalid or null.";
        return;
    }
    m_sslconfig.addCaCertificate(certificate);
    m_sslconfig.setProtocol(QSsl::TlsV1_2OrLater);

    // Create a Socket and apply the configuration
    m_tcpSocket = new QSslSocket(this);
    m_tcpSocket->setSslConfiguration(m_sslconfig);
    qDebug() << "Custom SSL configuration has been set on the socket.";

    connect(m_tcpSocket, &QTcpSocket::readyRead, this, &ChatClientHandler::onReadyRead);
    connect(m_tcpSocket, &QTcpSocket::connected, this, &ChatClientHandler::connected);
    connect(m_tcpSocket, &QTcpSocket::errorOccurred, this, &ChatClientHandler::onErrorOccurred);
    connect(m_tcpSocket, &QSslSocket::sslErrors, this, [](const QList<QSslError> &errors){
        qWarning() << "!!!!!! SSL Errors Occurred:";
        for (const QSslError &error : errors) {
            qWarning() << "  -" << error.errorString();
        }
    });
}

void ChatClientHandler::connectToServer(const QString &host, quint16 port)
{
    if (m_tcpSocket->state() == QAbstractSocket::UnconnectedState) {
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


