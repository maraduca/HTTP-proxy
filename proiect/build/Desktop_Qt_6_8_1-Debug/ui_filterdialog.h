/********************************************************************************
** Form generated from reading UI file 'filterdialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FILTERDIALOG_H
#define UI_FILTERDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableWidget>

QT_BEGIN_NAMESPACE

class Ui_FilterDialog
{
public:
    QLineEdit *lineEditUrl;
    QTableWidget *RulesTableWidget;
    QComboBox *ActionComboBox;
    QPushButton *addRuleButton;
    QPushButton *DeleteRuleButton;
    QPushButton *editRuleButton;
    QPushButton *SaveButton;

    void setupUi(QDialog *FilterDialog)
    {
        if (FilterDialog->objectName().isEmpty())
            FilterDialog->setObjectName("FilterDialog");
        FilterDialog->resize(832, 617);
        lineEditUrl = new QLineEdit(FilterDialog);
        lineEditUrl->setObjectName("lineEditUrl");
        lineEditUrl->setGeometry(QRect(80, 340, 331, 22));
        RulesTableWidget = new QTableWidget(FilterDialog);
        RulesTableWidget->setObjectName("RulesTableWidget");
        RulesTableWidget->setGeometry(QRect(10, 40, 791, 271));
        ActionComboBox = new QComboBox(FilterDialog);
        ActionComboBox->setObjectName("ActionComboBox");
        ActionComboBox->setGeometry(QRect(160, 400, 141, 23));
        addRuleButton = new QPushButton(FilterDialog);
        addRuleButton->setObjectName("addRuleButton");
        addRuleButton->setGeometry(QRect(630, 340, 80, 23));
        DeleteRuleButton = new QPushButton(FilterDialog);
        DeleteRuleButton->setObjectName("DeleteRuleButton");
        DeleteRuleButton->setGeometry(QRect(630, 390, 80, 23));
        editRuleButton = new QPushButton(FilterDialog);
        editRuleButton->setObjectName("editRuleButton");
        editRuleButton->setGeometry(QRect(630, 440, 80, 23));
        SaveButton = new QPushButton(FilterDialog);
        SaveButton->setObjectName("SaveButton");
        SaveButton->setGeometry(QRect(270, 520, 80, 23));

        retranslateUi(FilterDialog);

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
