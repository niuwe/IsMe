#include "logindialog.h"
#include "ui_logindialog.h"
#include <QMessageBox>
#include <QFile>

LoginDialog::LoginDialog(ChatClientHandler *handler, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
    , m_handler(handler)
{
    ui->setupUi(this);
    this->setWindowTitle("Login Client");

    // === 加载并应用QSS样式表 ===
    QFile styleFile(":/loginstyle.qss"); // 从资源文件加载
    if (styleFile.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(styleFile.readAll());
        this->setStyleSheet(styleSheet);
        styleFile.close();
    } else {
        qDebug() << "Warning: Could not open LoginStyle.qss";
    }

    connect(m_handler, &ChatClientHandler::connected, this, &LoginDialog::onConnected);
    connect(m_handler, &ChatClientHandler::errorOccurred, this, &LoginDialog::onError);
    connect(m_handler, &ChatClientHandler::jsonMessageReceived,
            this, &LoginDialog::onJsonReceived);

    if (!(m_handler->isConnected()))
    {
        ui->statusLabel->setText("Connecting to server...");
        m_handler->connectToServer("127.0.0.1", 12694);
    }
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_loginButton_clicked()
{
    QString username = ui->usernameLineEdit->text();
    QString password = ui->passwordLineEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Login failed",
                            "Username and password cannot be empty.");
        return;
    }

    QJsonObject loginJson;
    loginJson["type"] = "login";
    loginJson["username"] = username;
    loginJson["password"] = password;
    m_pendingMessage = loginJson;

    if (m_handler->isConnected())
    {
        onConnected();
    }else{
        ui->statusLabel->setText("Connecting to server...");
        m_handler->connectToServer("127.0.0.1", 12345);
    }
}

void LoginDialog::onConnected()
{
    ui->statusLabel->setText("Connected to server!");
    qDebug() << "LoginDialog: Connected to server!";

    // 检查是否有待发送的消息
    if (!m_pendingMessage.isEmpty()) {
        qDebug() << "Sending pending message:" << m_pendingMessage;
        m_handler->sendMessage(m_pendingMessage);
        m_pendingMessage = QJsonObject(); // 发送后立即清空，防止重发
    }
}

void LoginDialog::onError(const QString &errorString)
{
    ui->statusLabel->setText("Error: " + errorString);
    QMessageBox::warning(this, "Connection failed", "Error: " + errorString);
    qDebug() << "Error from handler:" << errorString;
}

QString LoginDialog::getUsername() const
{
    return m_username;
}

void LoginDialog::on_registerButton_clicked()
{
    QString username = ui->usernameLineEdit->text();
    QString password = ui->passwordLineEdit->text();
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Registration failed",
                             "Username and password cannot be empty.");
        return;
    }

    QJsonObject registerJson;
    registerJson["type"] = "register";
    registerJson["username"] = username;
    registerJson["password"] = password;
    m_pendingMessage = registerJson;

    if (m_handler->isConnected())
    {
        onConnected();
    }else{
        ui->statusLabel->setText("Connecting to server...");
        m_handler->connectToServer("127.0.0.1", 12694);
    }
}


void LoginDialog::on_showPasswordCheckBox_toggled(bool checked)
{
    if (checked) {
        ui->passwordLineEdit->setEchoMode(QLineEdit::Normal);
    } else {
        ui->passwordLineEdit->setEchoMode(QLineEdit::Password);
    }
}

// 統一處理所有收到的 JSON 消息
void LoginDialog::onJsonReceived(const QJsonObject &json)
{
    QString type = json["type"].toString();

    if (type == "login_success") {
        m_username = json["username"].toString();
        qDebug()<< "LoginDialog: log in successed!";
        accept(); // 關閉對話框，並返回 QDialog::Accepted
    } else if (type == "login_failure") {
        QMessageBox::warning(this, "Login Failed", json["reason"].toString());
        ui->passwordLineEdit->clear();
    } else if (type == "registration_success") {
        QMessageBox::information(this, "Registration Successful",
                                 "Your account has been successfully "
                                 "registered and can now be used to log in.");
    } else if (type == "registration_failure") {
        QMessageBox::warning(this, "Registration Failed", json["reason"].toString());
    }
}
