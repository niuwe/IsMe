#include "sslserver.h"

SslServer::SslServer(QObject *parent) : QTcpServer(parent)
{
}

void SslServer::setSslConfiguration(const QSslConfiguration &config)
{
    m_sslConfig = config;
}

void SslServer::incomingConnection(qintptr socketDescriptor)
{
    // Create a QSslSocket object
    QSslSocket *sslSocket = new QSslSocket(this);
    // Set socket descriptor
    if (sslSocket->setSocketDescriptor(socketDescriptor)) {
        // Apply SSL settings
        sslSocket->setSslConfiguration(m_sslConfig);
        // Start server encryption
        sslSocket->startServerEncryption();
        // Add the prepared socket to the pending connection queue
        addPendingConnection(sslSocket);
    } else {
        delete sslSocket;
    }
}
