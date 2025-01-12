/********************************************************************************
** Form generated from reading UI file 'httpproxy.ui'
**
** Created by: Qt User Interface Compiler version 5.15.13
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_HTTPPROXY_H
#define UI_HTTPPROXY_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>

QT_BEGIN_NAMESPACE

class Ui_httpproxy
{
public:
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *httpproxy)
    {
        if (httpproxy->objectName().isEmpty())
            httpproxy->setObjectName(QString::fromUtf8("httpproxy"));
        httpproxy->resize(400, 300);
        buttonBox = new QDialogButtonBox(httpproxy);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(30, 240, 341, 32));
        buttonBox->setOrientation(Qt::Orientation::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::StandardButton::Cancel|QDialogButtonBox::StandardButton::Ok);

        retranslateUi(httpproxy);
        QObject::connect(buttonBox, SIGNAL(accepted()), httpproxy, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), httpproxy, SLOT(reject()));

        QMetaObject::connectSlotsByName(httpproxy);
    } // setupUi

    void retranslateUi(QDialog *httpproxy)
    {
        httpproxy->setWindowTitle(QCoreApplication::translate("httpproxy", "Dialog", nullptr));
    } // retranslateUi

};

namespace Ui {
    class httpproxy: public Ui_httpproxy {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_HTTPPROXY_H
