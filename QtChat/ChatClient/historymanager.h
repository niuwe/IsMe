#ifndef HISTORYMANAGER_H
#define HISTORYMANAGER_H

#include <QObject>
#include <QJsonObject>
#include <QList>

class HistoryManager : public QObject
{
    Q_OBJECT
public:
    explicit HistoryManager(const QString &currentUsername, QObject *parent = nullptr);
    void saveMessage(const QString &peerUsername, const QJsonObject &message);
    QList<QJsonObject> loadHistory(const QString &peerUsername);

private:
    QString getLogFilePath(const QString &peerUsername) const;
    QString m_basePath;
};

#endif // HISTORYMANAGER_H
