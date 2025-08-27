#ifndef CHATCLIENTHANDLER_H
#define CHATCLIENTHANDLER_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>
#include <QSslSocket>

class ChatClientHandler : public QObject
{
    Q_OBJECT
public:
    explicit ChatClientHandler(QTcpSocket *socket, QObject *parent = nullptr);
    void connectToServer(const QString &host, quint16 port);
    bool isConnected() const;

public slots:
    void sendMessage(const QJsonObject &json);

signals:
    void jsonMessageReceived(const QJsonObject &json);
    void connected();
    void errorOccurred(const QString &errorString);

private slots:
    void onReadyRead();
    void onErrorOccurred(QAbstractSocket::SocketError socketError);

private:
    QTcpSocket* m_tcpSocket;
    //QSslSocket* m_tcpSocket;
    qint32 m_currentBlockSize;
};

#endif
