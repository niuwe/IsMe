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

    // ===Load and apply QSS style sheet ===
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
    connect(ui->userListWidget, &QListWidget::itemClicked,
            this, &MainWindow::onUserSelectionChanged);

    // In the initial state,
    //the input box and send button are disabled because no chat partner has been selected.
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

void MainWindow::onUserSelectionChanged(QListWidgetItem *item)
{
    if (!item) {
        m_currentPeer.clear();
        ui->chatDisplayBrowser->clear();
        ui->messageLineEdit->setEnabled(false);
        ui->sendButton->setEnabled(false);
        return;
    }

    // Updata the current chat user
    m_currentPeer = item->text();

    QFont font = item->font();
    font.setBold(false);
    item->setForeground(Qt::white);
    item->setFont(font);
    this->setWindowTitle(QString("Chat Client - %1 (與 %2 對話中)").arg(m_username,m_currentPeer));

    ui->chatDisplayBrowser->clear();

    // Load history with this user
    QList<QJsonObject> history = m_historyManager->loadHistory(m_currentPeer);
    for (const QJsonObject &message : std::as_const(history)) {
        displayMessage(message);
    }

    // enable send and input functions
    ui->messageLineEdit->setEnabled(true);
    ui->sendButton->setEnabled(true);
}



void MainWindow::on_sendButton_clicked()
{
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
    m_handler->sendMessage(messageJson);

    displayMessage(messageJson);

    m_historyManager->saveMessage(m_currentPeer, messageJson);

    ui->messageLineEdit->clear();
    ui->messageLineEdit->setFocus();
}


void MainWindow::onJsonReceived(const QJsonObject &json)
{
    QString type = json["type"].toString();
    if (type == "user_list_update") {
        handleUserListUpdate(json);
    } else if (type == "chat_message") {
        handleChatMessage(json);
    } else if (type == "message_failure") {
        ui->chatDisplayBrowser->append(QString("<font color='red'>system notification: %1</font>").arg(json["reason"].toString()));
    } else {
        qDebug() << "Received unknown message type:" << type;
    }
}


void MainWindow::handleUserListUpdate(const QJsonObject &json)
{
    QJsonArray users = json["users"].toArray();
    QString previouslySelectedUser = m_currentPeer;
    QListWidgetItem *itemToSelect = nullptr;

    ui->userListWidget->clear();

    for (const QJsonValue &userValue : std::as_const(users)) {
        const QString username = userValue.toString();
        if (username == m_username) {
            continue;
        }

        QListWidgetItem *newItem = new QListWidgetItem(username, ui->userListWidget);


        if (username == previouslySelectedUser) {
            itemToSelect = newItem;
        }
    }

    ui->userListWidget->setCurrentItem(itemToSelect);

    if (!itemToSelect && !previouslySelectedUser.isEmpty()) {
        onUserSelectionChanged(nullptr);
    }
}

void MainWindow::handleChatMessage(const QJsonObject &json)
{
    QString from = json["from"].toString();

    m_historyManager->saveMessage(from, json);

    // If the sender of the message is the person you are currently chatting with,
    // it will be displayed directly
    if (from == m_currentPeer) {
        displayMessage(json);
    } else {
        // Here you need to traverse the QListWidget to find the corresponding item and modify its font
        for (int i = 0; i < ui->userListWidget->count(); ++i) {
            QListWidgetItem *item = ui->userListWidget->item(i);
            if (item->text() == from) {
                QFont font = item->font();
                font.setBold(true);
                item->setFont(font);
                item->setForeground(Qt::red);
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

    content = content.toHtmlEscaped();
    content.replace("\n", "<br/>");

    QString formattedMessage;

    // user/Logo on the left
    // Timestamp on the right
    QString headerTemplate =
        "<table width='100%'><tr>"
        "<td align='left'>%1</td>"
        "<td align='right'><font color='gray'>%2</font></td>"
        "</tr></table>";

    if (from == m_username) {
        // Messages sent by yourself
        QString header = headerTemplate.arg("<b><font color='#2E8B57'>Me (我)</font></b>",timestamp);
        formattedMessage = header + content;

    } else {
        // Messages received
        QString header = headerTemplate.arg(QString("<b><font color='#7AC5CD'>%1</font></b>").arg(from),timestamp);
        formattedMessage = header + content;
    }

    ui->chatDisplayBrowser->append(formattedMessage);
}
