#include "logindialog.h"
#include "ui_logindialog.h"
#include <QMessageBox>

LoginDialog::LoginDialog(QTcpSocket *socket, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
    , m_tcpSocket(socket)
{
    ui->setupUi(this);
    this->setWindowTitle("Login Client");

    connect(m_tcpSocket, &QTcpSocket::connected, this, &LoginDialog::onConnected);
    connect(m_tcpSocket, &QTcpSocket::errorOccurred, this, &LoginDialog::onErrorOccurred);
    connect(m_tcpSocket, &QTcpSocket::readyRead, this, &LoginDialog::onReadyRead);

    if (m_tcpSocket->state() == QAbstractSocket::UnconnectedState)
    {
        ui->statusLabel->setText("正在連線至伺服器...");
        m_tcpSocket->connectToHost("127.0.0.1", 12345);
    }
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_loginButton_clicked()
{
    if (m_tcpSocket->state() != QAbstractSocket::ConnectedState)
    {
        ui->statusLabel->setText("Connecting...");
        m_tcpSocket->connectToHost("127.0.0.1", 12345);

        QString username = ui->usernameLineEdit->text();
        QString password = ui->passwordLineEdit->text();
        QJsonObject loginJson;
        loginJson["type"] = "login";
        loginJson["username"] = username;
        loginJson["password"] = password;

        sendMessage(loginJson);
        //QMessageBox::critical(this, "錯誤", "尚未連線至伺服器，請稍候。");
        return;
    }else{
        QString username = ui->usernameLineEdit->text();
        QString password = ui->passwordLineEdit->text();
        QJsonObject loginJson;
        loginJson["type"] = "login";
        loginJson["username"] = username;
        loginJson["password"] = password;
        sendMessage(loginJson);
    }
}

void LoginDialog::onReadyRead()
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
            break;
        }

        QByteArray jsonData = m_tcpSocket->read(blockSize);// 讀取包體
        qDebug() << "LoginDialog: received message is:" << jsonData;
        // 解析並分發消息
        QJsonDocument doc = QJsonDocument::fromJson(jsonData);
        if(doc.isObject())
        {
            QJsonObject json = doc.object();
            QString type = json["type"].toString();

            if (type == "login_success") {
                handleLoginSuccess(json);
            }else if(type == "login_failure"){
                handleLoginFailure(json);
            }else if (type == "registration_success") {
                handleRegistrationSuccess();
            } else if (type == "registration_failure") {
                handleRegistrationFailure(json);
            }

        }
    }
}

void LoginDialog::onConnected()
{
    ui->statusLabel->setText("Connected to server!");
    qDebug() << "LoginDialog: Connected to server!";

    QString username = ui->usernameLineEdit->text();
    QString password = ui->passwordLineEdit->text();

    QJsonObject loginJson;
    loginJson["type"] = "login";
    loginJson["username"] = username;
    loginJson["password"] = password;
    if((username == "") && (password == "")){


    }else
        sendMessage(loginJson);
}

void LoginDialog::sendMessage(const QJsonObject &json)
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

void LoginDialog::handleLoginSuccess(const QJsonObject &json)
{
    m_username = json["username"].toString();
    accept();
    qDebug()<< "LoginDialog: log in successed!";
}

void LoginDialog::onErrorOccurred(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError); // 避免 "unused parameter" 警告
    ui->statusLabel->setText("Error: " + m_tcpSocket->errorString());
    qDebug() << "Socket Error:" << m_tcpSocket->errorString();
}

void LoginDialog::handleLoginFailure(const QJsonObject &json)
{
    QMessageBox::warning(this, "登入失敗", json["reason"].toString());
    ui->passwordLineEdit->clear();
}


QString LoginDialog::getUsername() const
{
    return m_username;
}

void LoginDialog::on_registerButton_clicked()
{
    QString username = ui->usernameLineEdit->text();
    QString password = ui->passwordLineEdit->text();
    // 建立一個 "register" 類型的 JSON 訊息
    QJsonObject registerJson;
    registerJson["type"] = "register";
    registerJson["username"] = username;
    registerJson["password"] = password;

    if (m_tcpSocket->state() != QAbstractSocket::ConnectedState) {
        ui->statusLabel->setText("Connecting...");
        m_tcpSocket->connectToHost("127.0.0.1", 12345);

        sendMessage(registerJson);
        //QMessageBox::critical(this, "錯誤", "尚未連線至伺服器，請稍候。");
        return;
    }else if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "註冊失敗", "使用者名稱和密碼不能為空。");
        return;
    }else{
        sendMessage(registerJson);
    }
}

void LoginDialog::handleRegistrationSuccess()
{
    QMessageBox::information(this, "註冊成功", "您的帳號已成功註冊，現在可以使用它登入了。");
}

void LoginDialog::handleRegistrationFailure(const QJsonObject &json)
{
    QString reason = json["reason"].toString();
    QMessageBox::warning(this, "註冊失敗", reason);
}

void LoginDialog::on_showPasswordCheckBox_toggled(bool checked)
{
    if (checked) {
        // 如果勾選，將密碼框的 EchoMode 設為 Normal (正常顯示文字)
        ui->passwordLineEdit->setEchoMode(QLineEdit::Normal);
    } else {
        // 如果未勾選，設為 Password (顯示為 ●●●)
        ui->passwordLineEdit->setEchoMode(QLineEdit::Password);
    }
}
