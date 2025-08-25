#include "mainwindow.h"
#include "qmessagebox.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(ChatClientHandler *handler,
                       const QString &username, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_handler(handler)
    , m_username(username)

{
    ui->setupUi(this);
    this->setWindowTitle("Chat Client - " + m_username);

    connect(m_handler, &ChatClientHandler::jsonMessageReceived,
            this, &MainWindow::onJsonReceived);
    connect(ui->userListWidget, &QListWidget::currentItemChanged,
            this, &MainWindow::onUserSelectionChanged);

    // 初始狀態下，輸入框和發送按鈕不可用，因為還沒有選擇聊天對象
    ui->messageLineEdit->setEnabled(false);
    ui->sendButton->setEnabled(false);

    QJsonObject requestJson;
    requestJson["type"] = "request_user_list";
    m_handler->sendMessage(requestJson);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onUserSelectionChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    Q_UNUSED(previous);
    if (!current) {
        // 如果沒有選中項（例如列表為空），則清空並禁用
        m_currentPeer.clear();
        ui->chatDisplayBrowser->clear();
        ui->messageLineEdit->setEnabled(false);
        ui->sendButton->setEnabled(false);
        return;
    }

    // 1. 更新當前聊天對象
    m_currentPeer = current->text();
    this->setWindowTitle(QString("Chat Client - %1 (與 %2 對話中)").arg(m_username).arg(m_currentPeer));


    // 2. 清空聊天顯示窗口
    ui->chatDisplayBrowser->clear();

    // 3. 從 HistoryManager 加載與該用戶的歷史記錄
    QList<QJsonObject> history = m_historyManager->loadHistory(m_currentPeer);
    for (const QJsonObject &message : history) {
        displayMessage(message);
    }

    // 4. 啟用輸入和發送功能
    ui->messageLineEdit->setEnabled(true);
    ui->sendButton->setEnabled(true);
}



void MainWindow::on_sendButton_clicked()
{
    // 確保有選中的聊天對象
    if (m_currentPeer.isEmpty()) {
        return;
    }

    QString messageText = ui->messageLineEdit->text();
    if (messageText.isEmpty()) return;

    QJsonObject messageJson;
    messageJson["type"] = "chat_message";
    messageJson["from"] = m_username;
    messageJson["to"] = m_currentPeer;
    messageJson["content"] = messageText;

    // 1. 通過網絡發送
    m_handler->sendMessage(messageJson);

    // 2. 在本地立即顯示自己發送的消息
    displayMessage(messageJson);

    // 3. 將自己發送的消息保存到歷史記錄
    m_historyManager->saveMessage(m_currentPeer, messageJson);

    ui->messageLineEdit->clear();
    ui->messageLineEdit->setFocus();
}



//統一處理所有收到的 JSON 消息
void MainWindow::onJsonReceived(const QJsonObject &json)
{
    qDebug() << "[DEBUG] onJsonReceived triggered. Received JSON:" << json;

    QString type = json["type"].toString();
    qDebug() << "MainWindow received JSON type:" << type;

    if (type == "user_list_update") {
        handleUserListUpdate(json);
    } else if (type == "chat_message") {
        handleChatMessage(json);
    } else if (type == "message_failure") { // (可選) 處理消息發送失敗的回調
        ui->chatDisplayBrowser->append(QString("<font color='red'>系統提示: %1</font>").arg(json["reason"].toString()));
    } else {
        qDebug() << "Received unknown message type:" << type;
    }
}

void MainWindow::handleUserListUpdate(const QJsonObject &json)
{
    qDebug() << "[DEBUG] handleUserListUpdate triggered. Updating user list...";
    QJsonArray users = json["users"].toArray();
    QString previouslySelectedUser = "";
    if (ui->userListWidget->currentItem()) {
        previouslySelectedUser = ui->userListWidget->currentItem()->text();
    }

    // 為了防止切換選中狀態，我們先阻斷信號
    ui->userListWidget->blockSignals(true);
    ui->userListWidget->clear();

    for (const QJsonValue &userValue : std::as_const(users)) {
        const QString username = userValue.toString();
        if (username != m_username) {
            ui->userListWidget->addItem(username);
        }
    }

    ui->userListWidget->blockSignals(false);

    // 嘗試恢復之前的選擇
    if (!previouslySelectedUser.isEmpty()) {
        for (int i = 0; i < ui->userListWidget->count(); ++i) {
            QListWidgetItem *item = ui->userListWidget->item(i);
            if (item->text() == previouslySelectedUser) {
                // 找到了，手動設置為當前選中項
                ui->userListWidget->setCurrentItem(item);
                break;
            }
        }
    }

    // 如果恢復選擇失敗（例如用戶已下線），確保UI狀態正確
    if (!ui->userListWidget->currentItem()){
        onUserSelectionChanged(nullptr, nullptr); // 手動觸發清空和禁用邏輯
    }
}

void MainWindow::handleChatMessage(const QJsonObject &json)
{
    QString from = json["from"].toString();

    // 1. 無論當前是否正在與該用戶聊天，都先保存消息記錄
    m_historyManager->saveMessage(from, json);

    // 2. 如果收到消息的發送者，正好是當前正在聊天的對象，則直接顯示
    if (from == m_currentPeer) {
        displayMessage(json);
    } else {
        // (可選) 如果不是當前聊天對象，可以給予提示，例如將用戶列表中的名字加粗
        // 這裡需要遍歷 QListWidget 找到對應的 item 並修改其字體
        for (int i = 0; i < ui->userListWidget->count(); ++i) {
            QListWidgetItem *item = ui->userListWidget->item(i);
            if (item->text() == from) {
                QFont font = item->font();
                font.setBold(true);
                item->setFont(font);
                item->setForeground(Qt::red); // 也可以改變顏色
                break;
            }
        }
    }
}

void MainWindow::displayMessage(const QJsonObject &message)
{
    QString from = message["from"].toString();
    QString content = message["content"].toString();

    QString formattedMessage;
    if (from == m_username) {
        // 自己發送的消息
        formattedMessage = QString("<p style='text-align: right;'><b style=\"color:green;\">Me:</b> %1</p>").arg(content);
    } else {
        // 接收到的消息
        formattedMessage = QString("<p style='text-align: left;'><b style=\"color:blue;\">%1:</b> %2</p>").arg(from,content);
    }
    ui->chatDisplayBrowser->append(formattedMessage);
}
