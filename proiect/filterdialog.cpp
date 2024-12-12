#include "filterdialog.h"
#include "ui_filterdialog.h"

FilterDialog::FilterDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::FilterDialog)
{
    ui->setupUi(this);

    // Setări pentru combo box
    ui->ActionComboBox->addItems({"Allow", "Block"});

    // Configurare tabel
    ui->RulesTableWidget->setColumnCount(5);
    ui->RulesTableWidget->setHorizontalHeaderLabels(
        {"URL Pattern", "Header Key", "Header Value", "Body Pattern", "Action"});
    ui->RulesTableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->RulesTableWidget->setSelectionBehavior(QTableWidget::SelectRows);
    ui->RulesTableWidget->setEditTriggers(QTableWidget::NoEditTriggers);

    // Conectare butoane
    connect(ui->addRuleButton, &QPushButton::clicked, this, &FilterDialog::onAddRuleClicked);
    connect(ui->editRuleButton, &QPushButton::clicked, this, &FilterDialog::onEditRuleClicked);
    connect(ui->DeleteRuleButton, &QPushButton::clicked, this, &FilterDialog::onDeleteRuleClicked);
    connect(ui->SaveButton, &QPushButton::clicked, this, &FilterDialog::onSaveApplyClicked);
    //connect(ui->cancelButton, &QPushButton::clicked, this, &FilterDialog::reject);
}

FilterDialog::~FilterDialog()
{
    delete ui;
}

void FilterDialog::updateRulesTable(const QList<FilterRule> &rules)
{
    ui->RulesTableWidget->setRowCount(0); // Golește tabelul

    for (const FilterRule &rule : rules) {
        int row = ui->RulesTableWidget->rowCount();
        ui->RulesTableWidget->insertRow(row);

        ui->RulesTableWidget->setItem(row, 0, new QTableWidgetItem(rule.urlPattern));
        ui->RulesTableWidget->setItem(row, 1, new QTableWidgetItem(rule.headerKey));
        ui->RulesTableWidget->setItem(row, 2, new QTableWidgetItem(rule.headerValue));
       // ui->RulesTableWidget->setItem(row, 3, new QTableWidgetItem(rule.bodyPattern));
        ui->RulesTableWidget->setItem(row, 3, new QTableWidgetItem(rule.block ? "Block" : "Allow"));
    }
}

QList<FilterRule> FilterDialog::getRules() const
{
    QList<FilterRule> rules;

    for (int i = 0; i < ui->RulesTableWidget->rowCount(); ++i) {
        FilterRule rule;
        rule.urlPattern = ui->RulesTableWidget->item(i, 0)->text();
        rule.headerKey = ui->RulesTableWidget->item(i, 1)->text();
        rule.headerValue = ui->RulesTableWidget->item(i, 2)->text();
        rule.bodyPattern = ui->RulesTableWidget->item(i, 3)->text();
        rule.block = (ui->RulesTableWidget->item(i, 4)->text() == "Block");

        rules.append(rule);
    }

    return rules;
}

void FilterDialog::onAddRuleClicked()
{
    QString urlPattern = ui->lineEditUrl->text();
    QString headerKey = ui->lineEditHeaderKey->text();
    QString headerValue = ui->lineEditHeaderValue->text();
  //  QString bodyPattern = ui->lineEditBodyPattern->text();
    QString action = ui->ActionComboBox->currentText();

    int row = ui->RulesTableWidget->rowCount();
    ui->RulesTableWidget->insertRow(row);

    ui->RulesTableWidget->setItem(row, 0, new QTableWidgetItem(urlPattern));
    ui->RulesTableWidget->setItem(row, 1, new QTableWidgetItem(headerKey));
    ui->RulesTableWidget->setItem(row, 2, new QTableWidgetItem(headerValue));
   // ui->RulesTableWidget->setItem(row, 3, new QTableWidgetItem(bodyPattern));
    ui->RulesTableWidget->setItem(row, 4, new QTableWidgetItem(action));

    ui->lineEditUrl->clear();
    ui->lineEditHeaderKey->clear();
    ui->lineEditHeaderValue->clear();
  //  ui->lineEditBodyPattern->clear();
    ui->ActionComboBox->setCurrentIndex(0);
}

void FilterDialog::onEditRuleClicked()
{
    int selectedRow = ui->RulesTableWidget->currentRow();
    if (selectedRow < 0) return;

    ui->lineEditUrl->setText(ui->RulesTableWidget->item(selectedRow, 0)->text());
    ui->lineEditHeaderKey->setText(ui->RulesTableWidget->item(selectedRow, 1)->text());
    ui->lineEditHeaderValue->setText(ui->RulesTableWidget->item(selectedRow, 2)->text());
  //  ui->lineEditBodyPattern->setText(ui->RulesTableWidget->item(selectedRow, 3)->text());
    ui->ActionComboBox->setCurrentText(ui->RulesTableWidget->item(selectedRow, 4)->text());

    ui->RulesTableWidget->removeRow(selectedRow);
}

void FilterDialog::onDeleteRuleClicked()
{
    int selectedRow = ui->RulesTableWidget->currentRow();
    if (selectedRow >= 0) {
        ui->RulesTableWidget->removeRow(selectedRow);
    }
}

void FilterDialog::onSaveApplyClicked()
{
    emit rulesUpdated(getRules());
    qDebug()<<"Saved rules";
    accept();
}
