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

    // 將一條消息保存到與指定用戶的對話記錄中
    void saveMessage(const QString &peerUsername, const QJsonObject &message);

    // 從本地加載與指定用戶的完整對話記錄
    QList<QJsonObject> loadHistory(const QString &peerUsername);

private:
    // 獲取與指定用戶聊天記錄的文件路徑
    QString getLogFilePath(const QString &peerUsername) const;

    QString m_basePath; // 當前用戶的記錄存儲根目錄, e.g., "./chat_history/userA"
};

#endif // HISTORYMANAGER_H
