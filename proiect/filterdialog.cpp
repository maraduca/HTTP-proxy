#include "filterdialog.h"
#include "ui_filterdialog.h"

FilterDialog::FilterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FilterDialog)
{
    ui->setupUi(this);
    ui->methodComboBox->addItems({"GET", "POST", "CONNECT"});
}

FilterDialog::~FilterDialog()
{
    delete ui;
}

QString FilterDialog::getCriterion() const
{
    return ui->methodComboBox->currentText();
}

QString FilterDialog::getFilterText() const
{
    return ui->filterLineEdit->text();
}
