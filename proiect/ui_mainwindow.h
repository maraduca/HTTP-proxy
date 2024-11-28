/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.13
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
    QPushButton *openFiltersButton;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1244, 689);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        startButton = new QPushButton(centralwidget);
        startButton->setObjectName(QString::fromUtf8("startButton"));
        startButton->setGeometry(QRect(110, 560, 80, 23));
        firefoxButton = new QPushButton(centralwidget);
        firefoxButton->setObjectName(QString::fromUtf8("firefoxButton"));
        firefoxButton->setGeometry(QRect(320, 560, 80, 23));
        logTableWidget = new QTableWidget(centralwidget);
        logTableWidget->setObjectName(QString::fromUtf8("logTableWidget"));
        logTableWidget->setGeometry(QRect(50, 30, 441, 501));
        detailsTextEdit = new QTextEdit(centralwidget);
        detailsTextEdit->setObjectName(QString::fromUtf8("detailsTextEdit"));
        detailsTextEdit->setEnabled(true);
        detailsTextEdit->setGeometry(QRect(620, 170, 551, 401));
        detailsTextEdit->setAutoFillBackground(false);
        openFiltersButton = new QPushButton(centralwidget);
        openFiltersButton->setObjectName(QString::fromUtf8("openFiltersButton"));
        openFiltersButton->setGeometry(QRect(820, 50, 80, 23));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1244, 20));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        startButton->setText(QCoreApplication::translate("MainWindow", "startButton", nullptr));
        firefoxButton->setText(QCoreApplication::translate("MainWindow", "Firefox", nullptr));
        openFiltersButton->setText(QCoreApplication::translate("MainWindow", "PushButton", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
