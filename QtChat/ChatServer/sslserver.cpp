#include "sslserver.h"

SslServer::SslServer(QObject *parent) : QTcpServer(parent)
{
}

void SslServer::setSslConfiguration(const QSslConfiguration &config)
{
    m_sslConfig = config;
}

// 在這個方法中，我們將傳入的 socket 描述符直接建立為 QSslSocket
void SslServer::incomingConnection(qintptr socketDescriptor)
{
    // 建立一個 QSslSocket 物件
    QSslSocket *sslSocket = new QSslSocket(this);

    // 設定 socket 描述符
    if (sslSocket->setSocketDescriptor(socketDescriptor)) {
        // 套用 SSL 設定
        sslSocket->setSslConfiguration(m_sslConfig);
        // 啟動伺服器端加密
        sslSocket->startServerEncryption();
        // 將準備好的 socket 加入到待處理連線佇列中
        addPendingConnection(sslSocket);
    } else {
        delete sslSocket;
    }
}
