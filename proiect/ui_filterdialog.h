/********************************************************************************
** Form generated from reading UI file 'filterdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.15.13
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FILTERDIALOG_H
#define UI_FILTERDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableWidget>

QT_BEGIN_NAMESPACE

class Ui_FilterDialog
{
public:
    QDialogButtonBox *buttonBox;
    QLineEdit *lineEditUrl;
    QLineEdit *lineEditHeaderKey;
    QTableWidget *RulesTableWidget;
    QLineEdit *lineEditHeaderValue;
    QComboBox *ActionComboBox;
    QPushButton *addRuleButton;
    QPushButton *DeleteRuleButton;
    QPushButton *editRuleButton;
    QPushButton *SaveButton;

    void setupUi(QDialog *FilterDialog)
    {
        if (FilterDialog->objectName().isEmpty())
            FilterDialog->setObjectName(QString::fromUtf8("FilterDialog"));
        FilterDialog->resize(832, 617);
        buttonBox = new QDialogButtonBox(FilterDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(600, 540, 181, 32));
        buttonBox->setOrientation(Qt::Orientation::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::StandardButton::Cancel|QDialogButtonBox::StandardButton::Ok);
        lineEditUrl = new QLineEdit(FilterDialog);
        lineEditUrl->setObjectName(QString::fromUtf8("lineEditUrl"));
        lineEditUrl->setGeometry(QRect(80, 340, 331, 22));
        lineEditHeaderKey = new QLineEdit(FilterDialog);
        lineEditHeaderKey->setObjectName(QString::fromUtf8("lineEditHeaderKey"));
        lineEditHeaderKey->setGeometry(QRect(80, 380, 331, 22));
        RulesTableWidget = new QTableWidget(FilterDialog);
        RulesTableWidget->setObjectName(QString::fromUtf8("RulesTableWidget"));
        RulesTableWidget->setGeometry(QRect(10, 40, 791, 271));
        lineEditHeaderValue = new QLineEdit(FilterDialog);
        lineEditHeaderValue->setObjectName(QString::fromUtf8("lineEditHeaderValue"));
        lineEditHeaderValue->setGeometry(QRect(80, 420, 331, 22));
        ActionComboBox = new QComboBox(FilterDialog);
        ActionComboBox->setObjectName(QString::fromUtf8("ActionComboBox"));
        ActionComboBox->setGeometry(QRect(170, 470, 141, 23));
        addRuleButton = new QPushButton(FilterDialog);
        addRuleButton->setObjectName(QString::fromUtf8("addRuleButton"));
        addRuleButton->setGeometry(QRect(630, 340, 80, 23));
        DeleteRuleButton = new QPushButton(FilterDialog);
        DeleteRuleButton->setObjectName(QString::fromUtf8("DeleteRuleButton"));
        DeleteRuleButton->setGeometry(QRect(630, 390, 80, 23));
        editRuleButton = new QPushButton(FilterDialog);
        editRuleButton->setObjectName(QString::fromUtf8("editRuleButton"));
        editRuleButton->setGeometry(QRect(630, 440, 80, 23));
        SaveButton = new QPushButton(FilterDialog);
        SaveButton->setObjectName(QString::fromUtf8("SaveButton"));
        SaveButton->setGeometry(QRect(290, 570, 80, 23));

        retranslateUi(FilterDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), FilterDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), FilterDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(FilterDialog);
    } // setupUi

    void retranslateUi(QDialog *FilterDialog)
    {
        FilterDialog->setWindowTitle(QCoreApplication::translate("FilterDialog", "Dialog", nullptr));
        addRuleButton->setText(QCoreApplication::translate("FilterDialog", "Add Rule", nullptr));
        DeleteRuleButton->setText(QCoreApplication::translate("FilterDialog", "Delete Rule", nullptr));
        editRuleButton->setText(QCoreApplication::translate("FilterDialog", "Edit Rule", nullptr));
        SaveButton->setText(QCoreApplication::translate("FilterDialog", "Save", nullptr));
    } // retranslateUi

};

namespace Ui {
    class FilterDialog: public Ui_FilterDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FILTERDIALOG_H
