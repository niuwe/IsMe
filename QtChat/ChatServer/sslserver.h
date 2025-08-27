#ifndef SSLSERVER_H
#define SSLSERVER_H

#include <QTcpServer>
#include <QSslSocket>
#include <QSslConfiguration>

class SslServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit SslServer(QObject *parent = nullptr);
    void setSslConfiguration(const QSslConfiguration &config);

protected:
    // 重寫這個方法是關鍵
    void incomingConnection(qintptr socketDescriptor) override;

private:
    QSslConfiguration m_sslConfig;
};

#endif // SSLSERVER_H
