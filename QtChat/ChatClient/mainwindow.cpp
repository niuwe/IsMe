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

// 在 mainwindow.cpp 中

void MainWindow::handleUserListUpdate(const QJsonObject &json)
{
    QJsonArray users = json["users"].toArray();
    QString previouslySelectedUser = m_currentPeer;

    ui->userListWidget->blockSignals(true);

    ui->userListWidget->clear();

    // ======================= 全新核心修复策略 =======================
    // 1. 创建并添加一个临时的、不可见的占位符
    //    这会确保列表在添加第一个真实用户时不是空的，从而阻止自动选择行为。
    QListWidgetItem *placeholder = new QListWidgetItem();
    placeholder->setHidden(true); // 让用户看不见
    placeholder->setFlags(placeholder->flags() & ~Qt::ItemIsSelectable); // 禁止被选中
    ui->userListWidget->addItem(placeholder);
    // =============================================================

    QListWidgetItem *itemToSelect = nullptr;

    // 1. 正常添加所有真实的用户 (只管添加，不管选中)
    for (const QJsonValue &userValue : std::as_const(users)) {
        const QString username = userValue.toString();
        if (username != m_username) {
            // 直接创建并让 userListWidget 拥有它，不需要中间变量
            new QListWidgetItem(username, ui->userListWidget);
        }
    }

    // 3. 移除占位符。 takeItem会从列表中移除项，然后我们可以安全地删除它。
    delete ui->userListWidget->takeItem(ui->userListWidget->row(placeholder));


    // 现在列表处于干净的状态，我们可以安全地设置我们期望的选中项
    ui->userListWidget->setCurrentItem(itemToSelect);

    ui->userListWidget->blockSignals(false);

    // 处理之前聊天对象下线的情况
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
        QString header = headerTemplate.arg("<b><font color='#7AC5CD'>%1</font></b>",from,timestamp);
        formattedMessage = header + content;
    }

    // 使用最稳定、最简单的 append 方法添加新段落
    ui->chatDisplayBrowser->append(formattedMessage);
}
