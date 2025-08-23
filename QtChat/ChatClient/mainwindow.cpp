#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Chat Client");
    m_tcpSocket = new QTcpSocket(this);


    // 連接 socket 的信號到我們的槽
    connect(m_tcpSocket, &QTcpSocket::connected, this, &MainWindow::onConnected);
    connect(m_tcpSocket, &QTcpSocket::errorOccurred, this, &MainWindow::onErrorOccurred);
    connect(m_tcpSocket, &QTcpSocket::readyRead, this, &MainWindow::onReadyRead);

}

MainWindow::~MainWindow()
{
    delete ui;
}


// 登錄按鈕點擊
void MainWindow::on_loginButton_clicked()
{
    QString username = ui->usernameLineEdit->text();
    QString password = ui->passwordLineEdit->text();

    QJsonObject loginJson;
    loginJson["type"] = "login";
    loginJson["username"] = username;
    loginJson["password"] = password;


    sendMessage(loginJson);
}

// 發送按鈕點擊
void MainWindow::on_sendButton_clicked()
{
    qDebug() << "Send button clicked.";
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

    qDebug() << "Sending chat message to" << toUser;
    sendMessage(messageJson);

    // 在自己的聊天窗口也顯示自己發送的消息
    ui->chatDisplayBrowser->append(QString("[Me -> %1]: %2").arg(toUser).arg(messageText));
    ui->messageLineEdit->clear();
}

void MainWindow::onReadyRead()
{
    // 使用 QDataStream 來處理數據流
    QDataStream in(m_tcpSocket);
    in.setVersion(QDataStream::Qt_6_0);

    // 循環讀取，防止粘包（一次性收到多個包）
    while (true) {
        // 首先嘗試讀取包頭（數據大小）
        if (m_tcpSocket->bytesAvailable() < sizeof(qint32)) {
            break; // 數據不夠讀一個完整的包頭，退出循環等待下次數據
        }

        qint32 blockSize;
        in >> blockSize; // 從流中讀取包頭

        // 接著嘗試讀取包體（JSON數據）
        if (m_tcpSocket->bytesAvailable() < blockSize) {
            // 數據不完整，需要回退指針並等待
            // 為了簡化教程，我們暫不處理這種極端情況，假設包體總是一次性到達
            // 在一個健壯的客戶端中，這裡需要更複雜的緩衝區邏輯
            break;
        }

        //QByteArray jsonData;
        QByteArray jsonData = m_tcpSocket->read(blockSize);// 讀取包體

        // 解析並分發消息
        QJsonDocument doc = QJsonDocument::fromJson(jsonData);
        if(doc.isObject())
        {
            QJsonObject json = doc.object();
            QString type = json["type"].toString();

            if (type == "login_success") {
                handleLoginSuccess(json);
                qDebug()<< "received lgon_success";
            } else if (type == "login_failure") {
                handleLoginFailure(json);
            } else if (type == "user_list_update") {
                handleUserListUpdate(json);
            } else if (type == "chat_message") {
                handleChatMessage(json);
            }
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


void MainWindow::handleLoginSuccess(const QJsonObject &json)
{
    m_username = json["username"].toString();
    this->setWindowTitle("Chat Client - " + m_username);
    // 切換到聊天頁面
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::handleLoginFailure(const QJsonObject &json)
{
    ui->errorLabel->setText(json["reason"].toString());
}

void MainWindow::handleUserListUpdate(const QJsonObject &json)
{
    ui->userListWidget->clear();
    QJsonArray users = json["users"].toArray();
    for (const QJsonValue &user : users) {
        // 不在列表中顯示自己
        if (user.toString() != m_username) {
            ui->userListWidget->addItem(user.toString());
        }
    }
}

void MainWindow::handleChatMessage(const QJsonObject &json)
{
    QString from = json["from"].toString();
    QString content = json["content"].toString();
    ui->chatDisplayBrowser->append(QString("[%1->Me]: %2").arg(from).arg(content));
}

void MainWindow::on_connectButton_clicked()
{
    if (m_tcpSocket->state() == QAbstractSocket::UnconnectedState)
    {
        ui->statusLineEdit->setText("Connecting...");
        m_tcpSocket->connectToHost("127.0.0.1", 12345);
    }
}

void MainWindow::onErrorOccurred(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError); // 避免 "unused parameter" 警告
    ui->statusLineEdit->setText("Error: " + m_tcpSocket->errorString());
    qDebug() << "Socket Error:" << m_tcpSocket->errorString();
}

void MainWindow::onConnected()
{
    ui->statusLineEdit->setText("Connected to server!");
    qDebug() << "Connected to server!";
}
