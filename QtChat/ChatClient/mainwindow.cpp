#include "mainwindow.h"
#include "historymanager.h"
#include "chatclienthandler.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QDateTime>
#include <QTextCursor>
#include <QTextBlockFormat>
#include <QFile>

MainWindow::MainWindow(ChatClientHandler *handler,
                       const QString &username, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_handler(handler)
    , m_username(username)
    , m_historyManager(new HistoryManager(username, this))
{
    ui->setupUi(this);
    this->setWindowTitle("Chat Client - " + m_username);

    // === 加载并应用QSS样式表 ===
    QFile styleFile(":/mainwindowstyle.qss");
    if (styleFile.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(styleFile.readAll());
        this->setStyleSheet(styleSheet);
        styleFile.close();
    } else {
        qDebug() << "Warning: Could not open MainStyle.qss";
    }
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
        m_currentPeer.clear();
        ui->chatDisplayBrowser->clear();
        ui->messageLineEdit->setEnabled(false);
        ui->sendButton->setEnabled(false);
        return;
    }

    // 1. 更新當前聊天對象
    m_currentPeer = current->text();
    this->setWindowTitle(QString("Chat Client - %1 (與 %2 對話中)").arg(m_username,m_currentPeer));


    // 2. 清空聊天顯示窗口
    ui->chatDisplayBrowser->clear();

    // 3. 從 HistoryManager 加載與該用戶的歷史記錄
    QList<QJsonObject> history = m_historyManager->loadHistory(m_currentPeer);
    for (const QJsonObject &message : std::as_const(history)) {
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
    messageJson["timestamp"] = QDateTime::currentDateTime()
                                   .toString("yyyy-MM-dd hh:mm:ss");
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

    QString type = json["type"].toString();
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
    QJsonArray users = json["users"].toArray();
    // 1. 保存刷新前选中的用户名
    QString previouslySelectedUser = m_currentPeer;
    QListWidgetItem *itemToSelect = nullptr;

    // 在修改列表前，阻塞信号，防止currentItemChanged被频繁触发
    ui->userListWidget->blockSignals(true);

    // 2. 清空列表
    ui->userListWidget->clear();

    // 3. 遍历新用户列表，填充UI并查找需要重新选中的项
    for (const QJsonValue &userValue : std::as_const(users)) {
        const QString username = userValue.toString();
        // 不在列表中显示自己
        if (username == m_username) {
            continue;
        }

        // 创建新的列表项
        QListWidgetItem *newItem = new QListWidgetItem(username, ui->userListWidget);

        // 如果这个用户是之前选中的用户，就记录下来
        if (username == previouslySelectedUser) {
            itemToSelect = newItem;
        }
    }

    // 4. 恢复之前的选中状态（如果该用户仍然在线）
    // 如果 previouslySelectedUser 已下线, itemToSelect 将是 nullptr, 调用该函数会清空选择
    ui->userListWidget->setCurrentItem(itemToSelect);

    // 5. 恢复信号
    ui->userListWidget->blockSignals(false);

    // 6. （可选优化）如果选择被清空，手动触发一次逻辑更新
    // QListWidget 在 setCurrentItem(nullptr) 时，如果之前没有选中项，可能不发信号
    // 因此，为了确保UI（如聊天窗口）在用户下线时被正确清空，可以手动检查并调用。
    if (!itemToSelect && !previouslySelectedUser.isEmpty()) {
        onUserSelectionChanged(nullptr, nullptr);
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
    QString timestamp = message["timestamp"].toString();

    // 纠正之前的BUG：确保在处理前正确转义内容
    content = content.toHtmlEscaped();
    content.replace("\n", "<br/>");

    QString formattedMessage;

    // 使用HTML表格来确保布局的绝对稳定
    // 用户名/标识在左侧，时间戳在右侧
    QString headerTemplate =
        "<table width='100%'><tr>"
        "<td align='left'>%1</td>"
        "<td align='right'><font color='gray'>%2</font></td>"
        "</tr></table>";

    if (from == m_username) {
        // 自己发送的消息
        QString header = headerTemplate.arg("<b><font color='#2E8B57'>Me (我)</font></b>",timestamp);
        formattedMessage = header + content;

    } else {
        // 接收到的消息
        QString header = headerTemplate.arg(QString("<b><font color='#7AC5CD'>%1</font></b>").arg(from),timestamp);
        formattedMessage = header + content;
    }

    // 使用最稳定、最简单的 append 方法添加新段落
    ui->chatDisplayBrowser->append(formattedMessage);
}
