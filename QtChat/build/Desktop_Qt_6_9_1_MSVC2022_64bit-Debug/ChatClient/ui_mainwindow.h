/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QStackedWidget *stackedWidget;
    QWidget *page1;
    QLineEdit *usernameLineEdit;
    QLineEdit *passwordLineEdit;
    QPushButton *loginButton;
    QLabel *errorLabel;
    QWidget *page2;
    QListWidget *userListWidget;
    QTextBrowser *chatDisplayBrowser;
    QLineEdit *messageLineEdit;
    QPushButton *sendButton;
    QPushButton *connectButton;
    QLineEdit *statusLineEdit;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1280, 720);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        stackedWidget = new QStackedWidget(centralwidget);
        stackedWidget->setObjectName("stackedWidget");
        stackedWidget->setGeometry(QRect(160, 90, 951, 451));
        page1 = new QWidget();
        page1->setObjectName("page1");
        usernameLineEdit = new QLineEdit(page1);
        usernameLineEdit->setObjectName("usernameLineEdit");
        usernameLineEdit->setGeometry(QRect(220, 110, 381, 27));
        passwordLineEdit = new QLineEdit(page1);
        passwordLineEdit->setObjectName("passwordLineEdit");
        passwordLineEdit->setGeometry(QRect(220, 160, 381, 27));
        passwordLineEdit->setEchoMode(QLineEdit::EchoMode::Password);
        loginButton = new QPushButton(page1);
        loginButton->setObjectName("loginButton");
        loginButton->setGeometry(QRect(350, 220, 99, 28));
        errorLabel = new QLabel(page1);
        errorLabel->setObjectName("errorLabel");
        errorLabel->setGeometry(QRect(290, 80, 251, 21));
        errorLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);
        stackedWidget->addWidget(page1);
        page2 = new QWidget();
        page2->setObjectName("page2");
        userListWidget = new QListWidget(page2);
        userListWidget->setObjectName("userListWidget");
        userListWidget->setGeometry(QRect(20, 10, 241, 411));
        chatDisplayBrowser = new QTextBrowser(page2);
        chatDisplayBrowser->setObjectName("chatDisplayBrowser");
        chatDisplayBrowser->setGeometry(QRect(320, 10, 541, 351));
        messageLineEdit = new QLineEdit(page2);
        messageLineEdit->setObjectName("messageLineEdit");
        messageLineEdit->setGeometry(QRect(320, 380, 401, 27));
        sendButton = new QPushButton(page2);
        sendButton->setObjectName("sendButton");
        sendButton->setGeometry(QRect(760, 380, 99, 28));
        stackedWidget->addWidget(page2);
        connectButton = new QPushButton(centralwidget);
        connectButton->setObjectName("connectButton");
        connectButton->setGeometry(QRect(530, 630, 99, 28));
        statusLineEdit = new QLineEdit(centralwidget);
        statusLineEdit->setObjectName("statusLineEdit");
        statusLineEdit->setGeometry(QRect(390, 590, 411, 27));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1280, 24));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        stackedWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        loginButton->setText(QCoreApplication::translate("MainWindow", "log in", nullptr));
        errorLabel->setText(QCoreApplication::translate("MainWindow", "\350\253\213\350\274\270\345\205\245\350\263\254\350\231\237\345\222\214\345\257\206\347\242\274", nullptr));
        sendButton->setText(QCoreApplication::translate("MainWindow", "Send", nullptr));
        connectButton->setText(QCoreApplication::translate("MainWindow", "Connect", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
