#include "mainwindow.h"
#include "qmessagebox.h"
#include "ui_mainwindow.h"


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

    QJsonObject requestJson;
    requestJson["type"] = "request_user_list";
    m_handler->sendMessage(requestJson);

}

MainWindow::~MainWindow()
{
    delete ui;
}


// 發送按鈕點擊
void MainWindow::on_sendButton_clicked()
{
    qDebug() << "mainwondow: Send button clicked.";
    QString messageText = ui->messageLineEdit->text();
    if (messageText.isEmpty()) return;

    // 獲取當前在用戶列表中選中的用戶
    QListWidgetItem *selectedItem = ui->userListWidget->currentItem();
    if (!selectedItem) {
        qDebug() << "No user selected, returning.";
        // (可選) 提示用戶需要先選擇一個聊天對象
        QMessageBox::warning(this,"Erroe","No user selected ");
        return;
    }
    QString toUser = selectedItem->text();

    QJsonObject messageJson;
    messageJson["type"] = "chat_message";
    messageJson["from"] = m_username;
    messageJson["to"] = toUser;
    messageJson["content"] = messageText;

    qDebug() << "mainwindow: send chat message to" << toUser;
    //sendMessage(messageJson);
    m_handler->sendMessage(messageJson); // 修改：通過 handler 發送消息

    // 在自己的聊天窗口也顯示自己發送的消息
    ui->chatDisplayBrowser->append(QString("[Me -> %1]: %2").arg(toUser, messageText));
    ui->messageLineEdit->clear();
}


//統一處理所有收到的 JSON 消息
void MainWindow::onJsonReceived(const QJsonObject &json)
{
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
    qDebug() << "======= handleUserListUpdate() called! =======";
    qDebug() << "Received user list JSON:" << json;

    ui->userListWidget->clear();
    QJsonArray users = json["users"].toArray();

    qDebug() << "My username is:" << m_username << ". I will not be shown in the list.";
    qDebug() << "Users to be added:" << users;

    for (const QJsonValue &user : std::as_const(users)) {
        // 不在列表中顯示自己
        const QString username = user.toString();
        if (username != m_username) {
            qDebug() << "Adding user:" << username;
            ui->userListWidget->addItem(username);
        }
    }
}

void MainWindow::handleChatMessage(const QJsonObject &json)
{
    QString from = json["from"].toString();
    QString content = json["content"].toString();
    ui->chatDisplayBrowser->append(QString("[%1->Me]: %2").arg(from,content));
}

