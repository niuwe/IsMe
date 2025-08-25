#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAbstractSocket>
#include <QListWidgetItem>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
class ChatClientHandler;
class HistoryManager;

QT_BEGIN_NAMESPACE
namespace Ui {class MainWindow;}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

private slots:
    void on_sendButton_clicked();
    void onJsonReceived(const QJsonObject &json);
    void onUserSelectionChanged(QListWidgetItem *current, QListWidgetItem *previous);

private:

    void handleUserListUpdate(const QJsonObject &json);
    void handleChatMessage(const QJsonObject &json);
    void displayMessage(const QJsonObject &message);
public:
    ~MainWindow();
    explicit MainWindow(ChatClientHandler *handler,
                        const QString &username, QWidget *parent = nullptr);

private:
    Ui::MainWindow *ui;
    ChatClientHandler *m_handler;
    QString m_username;
    QString m_currentPeer;
    HistoryManager *m_historyManager;

};
#endif
