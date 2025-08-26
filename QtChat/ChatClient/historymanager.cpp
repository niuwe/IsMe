#include "historymanager.h"
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QStandardPaths>

HistoryManager::HistoryManager(const QString &currentUsername, QObject *parent)
    : QObject(parent)
{
    // Create a root folder to store history
    // C:\Users\***\AppData\Roaming\ChatClient\history
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    m_basePath = dataPath + "/history/" + currentUsername;

    QDir dir(m_basePath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
}

QString HistoryManager::getLogFilePath(const QString &peerUsername) const
{
    QStringList users = {m_basePath.split('/').last(), peerUsername};
    users.sort();
    return m_basePath + "/" + users.join("_") + ".jsonl";
}

void HistoryManager::saveMessage(const QString &peerUsername, const QJsonObject &message)
{
    QString filePath = getLogFilePath(peerUsername);
    QFile file(filePath);

    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
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
