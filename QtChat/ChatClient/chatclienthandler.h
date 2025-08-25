#ifndef CHATCLIENTHANDLER_H
#define CHATCLIENTHANDLER_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>

class ChatClientHandler : public QObject
{
    Q_OBJECT
public:
    explicit ChatClientHandler(QTcpSocket *socket, QObject *parent = nullptr);

public slots:
    void sendMessage(const QJsonObject &json);

signals:
    void jsonMessageReceived(const QJsonObject &json);
    void connected();
    void disconnected();
    void errorOccurred(const QString &errorString);

private slots:
    void onReadyRead();
    void onErrorOccurred(QAbstractSocket::SocketError socketError);

private:
    QTcpSocket* m_tcpSocket;
    qint32 m_currentBlockSize;
};

#endif
