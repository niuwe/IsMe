#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "chatclienthandler.h"

MainWindow::MainWindow(QTcpSocket *socket,
                       const QString &username, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_tcpSocket(socket)
    , m_username(username)
    , m_currentBlockSize(0)
{
    ui->setupUi(this);
    this->setWindowTitle("Chat Client - " + m_username);

    if (!m_tcpSocket) {
        // 防禦性程式設計，如果傳入的 socket 是空的，就直接返回
        qCritical() << "MainWindow received a null socket!";
        return;
    }

    disconnect(m_tcpSocket, &QTcpSocket::readyRead, nullptr, nullptr);
    connect(m_tcpSocket, &QTcpSocket::readyRead, this, &MainWindow::onReadyRead);

    QJsonObject requestJson;
    requestJson["type"] = "request_user_list";
    sendMessage(requestJson);

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
        return;
    }
    QString toUser = selectedItem->text();

    QJsonObject messageJson;
    messageJson["type"] = "chat_message";
    messageJson["from"] = m_username; // 使用已保存的自己的用戶名
    messageJson["to"] = toUser;
    messageJson["content"] = messageText;

    qDebug() << "mainwindow: send chat message to" << toUser;
    sendMessage(messageJson);

    // 在自己的聊天窗口也顯示自己發送的消息
    ui->chatDisplayBrowser->append(QString("[Me -> %1]: %2").arg(toUser, messageText));
    ui->messageLineEdit->clear();
}

// mainwindow.cpp

void MainWindow::onReadyRead()
{
    qDebug() << "======= MainWindow::onReadyRead() triggered! =======";
    QDataStream in(m_tcpSocket);
    in.setVersion(QDataStream::Qt_6_0); // 確保與伺服器/LoginDialog版本一致

    // 迴圈處理，因為一次 readyRead 可能包含多個完整的包
    while (true)
    {
        // 情況A：我們正在等待一個新的包頭
        if (m_currentBlockSize == 0) {
            // 檢查緩衝區中的數據是否足以讀取一個完整的包頭（4位元組）
            if (m_tcpSocket->bytesAvailable() < sizeof(qint32)) {
                qDebug() << "Not enough data for header, waiting...";
                return; // 數據不夠，直接返回，等待下一次 readyRead
            }
            // 讀取包頭，並儲存到成員變數中
            in >> m_currentBlockSize;
            qDebug() << "Header received, expecting body size =" << m_currentBlockSize;
        }

        // 情況B：我們正在等待包體
        // 檢查緩衝區中的數據是否足以讀取一個完整的包體
        if (m_tcpSocket->bytesAvailable() < m_currentBlockSize) {
            qDebug() << "Data not complete for body, waiting... Have"
                     << m_tcpSocket->bytesAvailable() << "bytes, but need" << m_currentBlockSize;
            return; // 數據不夠，直接返回，等待下一次 readyRead
        }

        // 讀取包體數據
        QByteArray jsonData = m_tcpSocket->read(m_currentBlockSize);
        qDebug() << "Read complete package (raw JSON):" << jsonData;

        m_currentBlockSize = 0;

        // 解析並分發消息
        QJsonDocument doc = QJsonDocument::fromJson(jsonData);
        if(doc.isObject())
        {
            QJsonObject json = doc.object();
            QString type = json["type"].toString();
            qDebug() << "Parsed JSON type:" << type;

            if (type == "user_list_update") {
                handleUserListUpdate(json);
            } else if (type == "chat_message") {
                handleChatMessage(json);
            } else {
                qDebug() << "Received unknown message type:" << type;
            }
        }
        else
        {
            qDebug() << "Failed to parse JSON data:" << jsonData;
        }

        // 檢查緩衝區是否還有剩餘的數據，如果有，迴圈將繼續處理下一個包
        if (m_tcpSocket->bytesAvailable() == 0) {
            break; // 緩衝區空了，退出迴圈
        }
    }
}

void MainWindow::sendMessage(const QJsonObject &json)
{

    if (m_tcpSocket->state() != QAbstractSocket::ConnectedState) {
        // 可以添加错误提示，例如 "尚未连接到服务器"
        return;
    }

    // 1. 将 JSON 对象转换为 QByteArray
    QByteArray data = QJsonDocument(json).toJson(QJsonDocument::Compact);

    // 2. 获取数据长度并转换为 QByteArray
    qint32 dataSize = data.size();
    QByteArray header;
    QDataStream stream(&header, QIODevice::WriteOnly);
    stream << dataSize;

    // 3. 发送包头，然后发送包体
    m_tcpSocket->write(header);
    m_tcpSocket->write(data);
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

