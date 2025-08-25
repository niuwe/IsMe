#include "historymanager.h"
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QStandardPaths>

HistoryManager::HistoryManager(const QString &currentUsername, QObject *parent)
    : QObject(parent)
{
    // 創建存儲歷史記錄的根文件夾，例如在用戶的應用數據目錄下
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    m_basePath = dataPath + "/history/" + currentUsername;

    QDir dir(m_basePath);
    if (!dir.exists()) {
        // mkpath可以遞歸創建文件夾
        dir.mkpath(".");
    }
}

QString HistoryManager::getLogFilePath(const QString &peerUsername) const
{
    // 文件名按字母排序，確保 userA->userB 和 userB->userA 的日誌是同一個文件
    QStringList users = {m_basePath.split('/').last(), peerUsername};
    users.sort();
    return m_basePath + "/" + users.join("_") + ".jsonl";
}

void HistoryManager::saveMessage(const QString &peerUsername, const QJsonObject &message)
{
    QString filePath = getLogFilePath(peerUsername);
    QFile file(filePath);

    // 以追加模式打開文件
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        // 將 JSON 對象轉換為緊湊的字符串，並在末尾添加換行符
        out << QJsonDocument(message).toJson(QJsonDocument::Compact) << "\n";
    }
}

QList<QJsonObject> HistoryManager::loadHistory(const QString &peerUsername)
{
    QList<QJsonObject> history;
    QString filePath = getLogFilePath(peerUsername);
    QFile file(filePath);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (!line.isEmpty()) {
                QJsonDocument doc = QJsonDocument::fromJson(line.toUtf8());
                if (doc.isObject()) {
                    history.append(doc.object());
                }
            }
        }
    }
    return history;
}
