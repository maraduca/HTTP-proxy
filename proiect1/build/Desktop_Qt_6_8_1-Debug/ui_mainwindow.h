/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QPushButton *startButton;
    QPushButton *firefoxButton;
    QTableWidget *logTableWidget;
    QTextEdit *detailsTextEdit;
    QPushButton *ForwardpushButton;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1244, 689);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        startButton = new QPushButton(centralwidget);
        startButton->setObjectName("startButton");
        startButton->setGeometry(QRect(140, 590, 80, 23));
        firefoxButton = new QPushButton(centralwidget);
        firefoxButton->setObjectName("firefoxButton");
        firefoxButton->setGeometry(QRect(360, 590, 80, 23));
        logTableWidget = new QTableWidget(centralwidget);
        logTableWidget->setObjectName("logTableWidget");
        logTableWidget->setGeometry(QRect(10, 80, 721, 471));
        detailsTextEdit = new QTextEdit(centralwidget);
        detailsTextEdit->setObjectName("detailsTextEdit");
        detailsTextEdit->setEnabled(true);
        detailsTextEdit->setGeometry(QRect(840, 140, 361, 461));
        detailsTextEdit->setAutoFillBackground(false);
        ForwardpushButton = new QPushButton(centralwidget);
        ForwardpushButton->setObjectName("ForwardpushButton");
        ForwardpushButton->setGeometry(QRect(130, 30, 131, 26));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1244, 23));
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
        startButton->setText(QCoreApplication::translate("MainWindow", "startButton", nullptr));
        firefoxButton->setText(QCoreApplication::translate("MainWindow", "Firefox", nullptr));
        ForwardpushButton->setText(QCoreApplication::translate("MainWindow", "Forward", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
