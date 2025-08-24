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
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_LoginDialog
{
public:
    QLabel *statusLabel;
    QPushButton *loginButton;
    QLineEdit *usernameLineEdit;
    QLineEdit *passwordLineEdit;

    void setupUi(QDialog *LoginDialog)
    {
        if (LoginDialog->objectName().isEmpty())
            LoginDialog->setObjectName("LoginDialog");
        LoginDialog->resize(320, 240);
        statusLabel = new QLabel(LoginDialog);
        statusLabel->setObjectName("statusLabel");
        statusLabel->setGeometry(QRect(70, 210, 161, 21));
        QFont font;
        font.setBold(true);
        font.setItalic(true);
        font.setUnderline(false);
        statusLabel->setFont(font);
        statusLabel->setCursor(QCursor(Qt::CursorShape::BlankCursor));
        statusLabel->setFrameShape(QFrame::Shape::NoFrame);
        statusLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);
        loginButton = new QPushButton(LoginDialog);
        loginButton->setObjectName("loginButton");
        loginButton->setGeometry(QRect(120, 170, 81, 24));
        usernameLineEdit = new QLineEdit(LoginDialog);
        usernameLineEdit->setObjectName("usernameLineEdit");
        usernameLineEdit->setGeometry(QRect(62, 70, 211, 23));
        passwordLineEdit = new QLineEdit(LoginDialog);
        passwordLineEdit->setObjectName("passwordLineEdit");
        passwordLineEdit->setGeometry(QRect(60, 120, 211, 23));

        retranslateUi(LoginDialog);

        QMetaObject::connectSlotsByName(LoginDialog);
    } // setupUi

    void retranslateUi(QDialog *LoginDialog)
    {
        LoginDialog->setWindowTitle(QCoreApplication::translate("LoginDialog", "Dialog", nullptr));
        statusLabel->setText(QCoreApplication::translate("LoginDialog", "wait connect...", nullptr));
        loginButton->setText(QCoreApplication::translate("LoginDialog", "Log in", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LoginDialog: public Ui_LoginDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINDIALOG_H
