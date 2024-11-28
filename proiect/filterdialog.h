#ifndef FILTERDIALOG_H
#define FILTERDIALOG_H

#include <QDialog>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLineEdit>

namespace Ui {
class FilterDialog;
}

class FilterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FilterDialog(QWidget *parent = nullptr);
    ~FilterDialog();
    QString getCriterion() const;
    QString getFilterText() const;
private:
    Ui::FilterDialog *ui;

};

#endif // FILTERDIALOG_H
