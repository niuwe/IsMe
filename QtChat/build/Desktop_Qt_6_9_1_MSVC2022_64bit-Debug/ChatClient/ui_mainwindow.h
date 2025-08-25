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
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout_3;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout_2;
    QLabel *label;
    QListWidget *userListWidget;
    QVBoxLayout *verticalLayout;
    QVBoxLayout *verticalLayout_3;
    QLabel *label_2;
    QTextBrowser *chatDisplayBrowser;
    QHBoxLayout *horizontalLayout_2;
    QLineEdit *messageLineEdit;
    QPushButton *sendButton;
    QLabel *statusLabel;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(800, 600);
        MainWindow->setStyleSheet(QString::fromUtf8("/* === \344\270\273\347\252\227\345\217\243\345\222\214\350\203\214\346\231\257 === */\n"
"QMainWindow#MainWindow, QWidget#centralwidget {\n"
"    background-color: #2D3035; /* \346\267\261\347\201\260\350\211\262\350\203\214\346\231\257 */\n"
"}\n"
"\n"
"/* === \347\224\250\346\210\267\345\210\227\350\241\250 (QListWidget) === */\n"
"QListWidget {\n"
"    background-color: #3C3F44; /* \346\257\224\350\203\214\346\231\257\347\250\215\344\272\256\347\232\204\347\201\260\350\211\262 */\n"
"    color: #E0E0E0; /* \345\210\227\350\241\250\346\226\207\345\255\227\351\242\234\350\211\262 */\n"
"    border: 1px solid #222; /* \346\233\264\346\267\261\347\232\204\350\276\271\346\241\206 */\n"
"    border-radius: 5px;\n"
"    padding: 5px; /* \345\210\227\350\241\250\347\232\204\345\206\205\350\276\271\350\267\235 */\n"
"    font-size: 14px;\n"
"}\n"
"/* \345\210\227\350\241\250\344\270\255\347\232\204\346\257\217\344\270\200\351\241\271 */\n"
"QListWidget::item {\n"
"    padding: 8px; /* \346\257\217\344\270\200\351\241"
                        "\271\347\232\204\345\206\205\350\276\271\350\267\235 */\n"
"    border-radius: 4px; /* \351\241\271\347\232\204\345\234\206\350\247\222 */\n"
"}\n"
"/* \351\274\240\346\240\207\346\202\254\345\201\234\345\234\250\346\237\220\344\270\200\351\241\271\344\270\212 */\n"
"QListWidget::item:hover {\n"
"    background-color: #4A4E54; /* \346\202\254\345\201\234\346\227\266\350\203\214\346\231\257\345\217\230\344\272\256 */\n"
"}\n"
"/* \351\200\211\344\270\255\346\237\220\344\270\200\351\241\271\346\227\266 */\n"
"QListWidget::item:selected {\n"
"    background-color: #007ACC; /* \350\223\235\350\211\262\351\253\230\344\272\256 */\n"
"    color: white; /* \351\200\211\344\270\255\346\227\266\346\226\207\345\255\227\345\217\230\347\231\275 */\n"
"}\n"
"\n"
"/* === \350\201\212\345\244\251\346\230\276\347\244\272\347\252\227\345\217\243 (QTextBrowser) === */\n"
"QTextBrowser {\n"
"    background-color: #25282C; /* \346\233\264\346\267\261\347\232\204\350\203\214\346\231\257\357\274\214\344\270\216\350\276\223\345\205\245"
                        "\345\214\272\345\210\206\345\274\200 */\n"
"    color: #F0F0F0;\n"
"    border: 1px solid #222;\n"
"    border-radius: 5px;\n"
"    padding: 8px; /* \345\206\205\350\276\271\350\267\235 */\n"
"    font-size: 14px;\n"
"}\n"
"\n"
"/* === \346\266\210\346\201\257\350\276\223\345\205\245\346\241\206 (QLineEdit) === */\n"
"QLineEdit {\n"
"    background-color: #3C3F44;\n"
"    color: #F0F0F0;\n"
"    border: 1px solid #555;\n"
"    border-radius: 5px;\n"
"    padding: 8px;\n"
"    font-size: 15px;\n"
"}\n"
"/* \350\276\223\345\205\245\346\241\206\350\216\267\345\217\226\347\204\246\347\202\271\346\227\266\347\232\204\346\240\267\345\274\217 */\n"
"QLineEdit:focus {\n"
"    border: 1px solid #007ACC;\n"
"    background-color: #454950;\n"
"}\n"
"\n"
"/* === \345\217\221\351\200\201\346\214\211\351\222\256 (QPushButton) === */\n"
"QPushButton#sendButton {\n"
"    background-color: #007ACC;\n"
"    color: white;\n"
"    border: none;\n"
"    border-radius: 5px;\n"
"    padding: 8px 16px; /* \345\267\246\345\217\263\345"
                        "\206\205\350\276\271\350\267\235\346\233\264\345\244\247 */\n"
"    font-size: 14px;\n"
"    font-weight: bold;\n"
"}\n"
"QPushButton#sendButton:hover {\n"
"    background-color: #008AEF;\n"
"}\n"
"QPushButton#sendButton:pressed {\n"
"    background-color: #006ABF;\n"
"}\n"
"\n"
"/* === \346\273\232\345\212\250\346\235\241\351\200\232\347\224\250\346\240\267\345\274\217 (QScrollBar) === */\n"
"QScrollBar:vertical {\n"
"    border: none;\n"
"    background: #2D3035; /* \346\273\232\345\212\250\346\235\241\350\203\214\346\231\257\350\211\262 */\n"
"    width: 10px; /* \346\273\232\345\212\250\346\235\241\345\256\275\345\272\246 */\n"
"    margin: 0px 0px 0px 0px;\n"
"}\n"
"QScrollBar::handle:vertical {\n"
"    background: #555; /* \346\273\232\345\212\250\346\235\241\346\273\221\345\235\227\351\242\234\350\211\262 */\n"
"    min-height: 20px; /* \346\273\221\345\235\227\346\234\200\345\260\217\351\253\230\345\272\246 */\n"
"    border-radius: 5px;\n"
"}\n"
"QScrollBar::handle:vertical:hover {\n"
"    background:"
                        " #777; /* \346\202\254\345\201\234\346\227\266\346\273\221\345\235\227\351\242\234\350\211\262 */\n"
"}\n"
"/* \351\232\220\350\227\217\344\270\212\344\270\213\347\256\255\345\244\264 */\n"
"QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {\n"
"    height: 0px;\n"
"}\n"
"\n"
"/* === \350\217\234\345\215\225\346\240\217\345\222\214\347\212\266\346\200\201\346\240\217 === */\n"
"QMenuBar {\n"
"    background-color: #2D3035;\n"
"    color: #E0E0E0;\n"
"}\n"
"QMenuBar::item {\n"
"    background: transparent;\n"
"    padding: 4px 8px;\n"
"}\n"
"QMenuBar::item:selected {\n"
"    background: #4A4E54;\n"
"}\n"
"QMenu {\n"
"    background-color: #3C3F44;\n"
"    color: #E0E0E0;\n"
"    border: 1px solid #222;\n"
"}\n"
"QMenu::item:selected {\n"
"    background-color: #007ACC;\n"
"}\n"
"QStatusBar {\n"
"    background-color: #2D3035;\n"
"    color: #A0A0A0;\n"
"}"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        horizontalLayout_3 = new QHBoxLayout(centralwidget);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName("verticalLayout_2");
        label = new QLabel(centralwidget);
        label->setObjectName("label");
        QFont font;
        font.setPointSize(13);
        font.setBold(true);
        font.setItalic(false);
        font.setUnderline(false);
        label->setFont(font);

        verticalLayout_2->addWidget(label);

        userListWidget = new QListWidget(centralwidget);
        userListWidget->setObjectName("userListWidget");

        verticalLayout_2->addWidget(userListWidget);

        verticalLayout_2->setStretch(1, 1);

        horizontalLayout->addLayout(verticalLayout_2);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName("verticalLayout_3");
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName("label_2");
        QFont font1;
        font1.setPointSize(13);
        font1.setBold(true);
        font1.setItalic(false);
        label_2->setFont(font1);

        verticalLayout_3->addWidget(label_2);


        verticalLayout->addLayout(verticalLayout_3);

        chatDisplayBrowser = new QTextBrowser(centralwidget);
        chatDisplayBrowser->setObjectName("chatDisplayBrowser");

        verticalLayout->addWidget(chatDisplayBrowser);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        messageLineEdit = new QLineEdit(centralwidget);
        messageLineEdit->setObjectName("messageLineEdit");

        horizontalLayout_2->addWidget(messageLineEdit);

        sendButton = new QPushButton(centralwidget);
        sendButton->setObjectName("sendButton");

        horizontalLayout_2->addWidget(sendButton);


        verticalLayout->addLayout(horizontalLayout_2);

        verticalLayout->setStretch(1, 1);

        horizontalLayout->addLayout(verticalLayout);

        horizontalLayout->setStretch(0, 1);
        horizontalLayout->setStretch(1, 3);

        horizontalLayout_3->addLayout(horizontalLayout);

        statusLabel = new QLabel(centralwidget);
        statusLabel->setObjectName("statusLabel");

        horizontalLayout_3->addWidget(statusLabel);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 24));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "User list \342\206\223", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "Message log \342\206\223", nullptr));
        sendButton->setText(QCoreApplication::translate("MainWindow", "Send", nullptr));
        statusLabel->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
