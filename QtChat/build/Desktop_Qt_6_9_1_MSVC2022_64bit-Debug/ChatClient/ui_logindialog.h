/********************************************************************************
** Form generated from reading UI file 'logindialog.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGINDIALOG_H
#define UI_LOGINDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LoginDialog
{
public:
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *horizontalLayout;
    QPushButton *loginButton;
    QPushButton *registerButton;
    QWidget *horizontalLayoutWidget_2;
    QHBoxLayout *horizontalLayout_2;
    QLabel *StatusLabel;
    QLabel *statusLabel;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QLineEdit *usernameLineEdit;
    QLineEdit *passwordLineEdit;
    QCheckBox *showPasswordCheckBox;

    void setupUi(QDialog *LoginDialog)
    {
        if (LoginDialog->objectName().isEmpty())
            LoginDialog->setObjectName("LoginDialog");
        LoginDialog->resize(491, 341);
        LoginDialog->setSizeIncrement(QSize(0, 0));
        LoginDialog->setStyleSheet(QString::fromUtf8("/* === \346\225\264\344\275\223\347\252\227\345\217\243\346\240\267\345\274\217 === */\n"
"QDialog#LoginDialog {\n"
"    background-color: #2D3035; /* \346\267\261\347\201\260\350\211\262\350\203\214\346\231\257 */\n"
"}\n"
"\n"
"/* === \346\240\207\347\255\276\351\200\232\347\224\250\346\240\267\345\274\217 === */\n"
"QLabel {\n"
"    color: #E0E0E0; /* \346\265\205\347\201\260\350\211\262\346\226\207\345\255\227 */\n"
"    font-size: 14px;\n"
"}\n"
"\n"
"/* === \350\276\223\345\205\245\346\241\206\346\240\267\345\274\217 (QLineEdit) === */\n"
"QLineEdit {\n"
"    background-color: #3C3F44; /* \346\257\224\350\203\214\346\231\257\347\250\215\344\272\256\347\232\204\347\201\260\350\211\262 */\n"
"    color: #F0F0F0; /* \350\276\223\345\205\245\346\226\207\345\255\227\351\242\234\350\211\262 */\n"
"    border: 1px solid #555; /* \350\276\271\346\241\206 */\n"
"    border-radius: 5px; /* \345\234\206\350\247\222 */\n"
"    padding: 8px; /* \345\206\205\350\276\271\350\267\235\357\274\214\350\256\251\346\226\207\345"
                        "\255\227\344\270\215\350\264\264\350\276\271 */\n"
"    font-size: 15px;\n"
"}\n"
"/* \350\276\223\345\205\245\346\241\206\350\216\267\345\217\226\347\204\246\347\202\271\346\227\266\347\232\204\346\240\267\345\274\217 */\n"
"QLineEdit:focus {\n"
"    border: 1px solid #007ACC; /* \350\223\235\350\211\262\351\253\230\344\272\256\350\276\271\346\241\206 */\n"
"    background-color: #454950;\n"
"}\n"
"\n"
"/* === \345\244\215\351\200\211\346\241\206\346\240\267\345\274\217 (QCheckBox) === */\n"
"QCheckBox {\n"
"    color: #E0E0E0; /* \"show password\" \346\226\207\345\255\227\351\242\234\350\211\262 */\n"
"    spacing: 8px; /* \345\244\215\351\200\211\346\241\206\344\270\216\346\226\207\345\255\227\347\232\204\351\227\264\350\267\235 */\n"
"}\n"
"/* \345\244\215\351\200\211\346\241\206\347\232\204\346\226\271\346\241\206\346\240\267\345\274\217 */\n"
"QCheckBox::indicator {\n"
"    width: 18px;\n"
"    height: 18px;\n"
"    border: 1px solid #555;\n"
"    border-radius: 4px;\n"
"    background-color: #3C3F44;\n"
""
                        "}\n"
"/* \345\244\215\351\200\211\346\241\206\346\202\254\345\201\234\346\227\266\347\232\204\346\240\267\345\274\217 */\n"
"QCheckBox::indicator:hover {\n"
"    border: 1px solid #777;\n"
"}\n"
"/* \345\244\215\351\200\211\346\241\206\350\242\253\351\200\211\344\270\255\346\227\266\347\232\204\346\240\267\345\274\217 */\n"
"QCheckBox::indicator:checked {\n"
"    background-color: #007ACC; /* \345\217\230\344\270\272\350\223\235\350\211\262 */\n"
"    border: 1px solid #007ACC;\n"
"    /* (\345\217\257\351\200\211) \346\267\273\345\212\240\344\270\200\344\270\252\345\261\205\344\270\255\347\232\204\347\231\275\350\211\262\345\257\271\345\213\276\345\233\276\346\240\207 */\n"
"    image: url(:/icons/check.png); \n"
"}\n"
"\n"
"/* === \346\214\211\351\222\256\351\200\232\347\224\250\346\240\267\345\274\217 === */\n"
"QPushButton {\n"
"    padding: 10px;\n"
"    font-size: 14px;\n"
"    border-radius: 5px;\n"
"    font-weight: bold;\n"
"}\n"
"\n"
"/* === \344\270\273\350\246\201\346\214\211\351\222\256\346\240\267"
                        "\345\274\217 (\347\231\273\345\275\225\346\214\211\351\222\256) === */\n"
"QPushButton#loginButton {\n"
"    background-color: #007ACC; /* \350\223\235\350\211\262\350\203\214\346\231\257 */\n"
"    color: white; /* \347\231\275\350\211\262\346\226\207\345\255\227 */\n"
"    border: none;\n"
"}\n"
"/* \351\274\240\346\240\207\346\202\254\345\201\234 */\n"
"QPushButton#loginButton:hover {\n"
"    background-color: #008AEF;\n"
"}\n"
"/* \351\274\240\346\240\207\346\214\211\344\270\213 */\n"
"QPushButton#loginButton:pressed {\n"
"    background-color: #006ABF;\n"
"}\n"
"\n"
"/* === \346\254\241\350\246\201\346\214\211\351\222\256\346\240\267\345\274\217 (\346\263\250\345\206\214\346\214\211\351\222\256) === */\n"
"QPushButton#registerButton {\n"
"    background-color: transparent; /* \351\200\217\346\230\216\350\203\214\346\231\257 */\n"
"    color: #007ACC; /* \350\223\235\350\211\262\346\226\207\345\255\227 */\n"
"    border: 1px solid #007ACC; /* \350\223\235\350\211\262\350\276\271\346\241\206 */\n"
"}\n"
"/*"
                        " \351\274\240\346\240\207\346\202\254\345\201\234 */\n"
"QPushButton#registerButton:hover {\n"
"    background-color: #007ACC;\n"
"    color: white;\n"
"}\n"
"/* \351\274\240\346\240\207\346\214\211\344\270\213 */\n"
"QPushButton#registerButton:pressed {\n"
"    background-color: #006ABF;\n"
"    border-color: #006ABF;\n"
"    color: white;\n"
"}\n"
"\n"
"/* === \347\212\266\346\200\201\346\240\217\346\240\207\347\255\276\346\240\267\345\274\217 === */\n"
"QLabel#statusLabel {\n"
"    color: #A0A0A0; /* \346\233\264\346\232\227\347\232\204\347\201\260\350\211\262\357\274\214\344\270\215\351\202\243\344\271\210\346\230\276\347\234\274 */\n"
"    font-style: italic;\n"
"}"));
        horizontalLayoutWidget = new QWidget(LoginDialog);
        horizontalLayoutWidget->setObjectName("horizontalLayoutWidget");
        horizontalLayoutWidget->setGeometry(QRect(70, 210, 331, 42));
        horizontalLayout = new QHBoxLayout(horizontalLayoutWidget);
        horizontalLayout->setSpacing(49);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setSizeConstraint(QLayout::SizeConstraint::SetMinAndMaxSize);
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        loginButton = new QPushButton(horizontalLayoutWidget);
        loginButton->setObjectName("loginButton");

        horizontalLayout->addWidget(loginButton);

        registerButton = new QPushButton(horizontalLayoutWidget);
        registerButton->setObjectName("registerButton");

        horizontalLayout->addWidget(registerButton);

        horizontalLayoutWidget_2 = new QWidget(LoginDialog);
        horizontalLayoutWidget_2->setObjectName("horizontalLayoutWidget_2");
        horizontalLayoutWidget_2->setGeometry(QRect(10, 290, 461, 26));
        horizontalLayout_2 = new QHBoxLayout(horizontalLayoutWidget_2);
        horizontalLayout_2->setSpacing(0);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        StatusLabel = new QLabel(horizontalLayoutWidget_2);
        StatusLabel->setObjectName("StatusLabel");
        QFont font;
        font.setBold(true);
        StatusLabel->setFont(font);
        StatusLabel->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        horizontalLayout_2->addWidget(StatusLabel);

        statusLabel = new QLabel(horizontalLayoutWidget_2);
        statusLabel->setObjectName("statusLabel");
        statusLabel->setEnabled(true);
        QFont font1;
        font1.setBold(true);
        font1.setItalic(true);
        font1.setUnderline(false);
        statusLabel->setFont(font1);
        statusLabel->setCursor(QCursor(Qt::CursorShape::BlankCursor));
        statusLabel->setFrameShape(QFrame::Shape::NoFrame);
        statusLabel->setAlignment(Qt::AlignmentFlag::AlignLeading|Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);

        horizontalLayout_2->addWidget(statusLabel);

        horizontalLayout_2->setStretch(1, 1);
        verticalLayoutWidget = new QWidget(LoginDialog);
        verticalLayoutWidget->setObjectName("verticalLayoutWidget");
        verticalLayoutWidget->setGeometry(QRect(70, 50, 331, 151));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setSpacing(9);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setSizeConstraint(QLayout::SizeConstraint::SetDefaultConstraint);
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        usernameLineEdit = new QLineEdit(verticalLayoutWidget);
        usernameLineEdit->setObjectName("usernameLineEdit");

        verticalLayout->addWidget(usernameLineEdit);

        passwordLineEdit = new QLineEdit(verticalLayoutWidget);
        passwordLineEdit->setObjectName("passwordLineEdit");
        passwordLineEdit->setContextMenuPolicy(Qt::ContextMenuPolicy::NoContextMenu);
        passwordLineEdit->setEchoMode(QLineEdit::EchoMode::Password);

        verticalLayout->addWidget(passwordLineEdit);

        showPasswordCheckBox = new QCheckBox(verticalLayoutWidget);
        showPasswordCheckBox->setObjectName("showPasswordCheckBox");
        showPasswordCheckBox->setAcceptDrops(false);
        showPasswordCheckBox->setLayoutDirection(Qt::LayoutDirection::LeftToRight);
        showPasswordCheckBox->setAutoFillBackground(false);

        verticalLayout->addWidget(showPasswordCheckBox);

        verticalLayout->setStretch(0, 1);
        verticalLayout->setStretch(1, 1);

        retranslateUi(LoginDialog);

        QMetaObject::connectSlotsByName(LoginDialog);
    } // setupUi

    void retranslateUi(QDialog *LoginDialog)
    {
        LoginDialog->setWindowTitle(QCoreApplication::translate("LoginDialog", "Dialog", nullptr));
        loginButton->setText(QCoreApplication::translate("LoginDialog", "Log in", nullptr));
        registerButton->setText(QCoreApplication::translate("LoginDialog", "register", nullptr));
        StatusLabel->setText(QCoreApplication::translate("LoginDialog", "server status info:", nullptr));
        statusLabel->setText(QString());
        showPasswordCheckBox->setText(QCoreApplication::translate("LoginDialog", "show password", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LoginDialog: public Ui_LoginDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINDIALOG_H
