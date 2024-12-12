#ifndef FILTERDIALOG_H
#define FILTERDIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QList>
#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>

// Structura pentru o regulă de filtrare
struct FilterRule {
    QString urlPattern;
    QString headerKey;
    QString headerValue;
    QString bodyPattern;
    bool block; // true = Block, false = Allow

    QJsonObject toJson() const {
        QJsonObject obj;
        obj["urlPattern"] = urlPattern;
        obj["headerKey"] = headerKey;
        obj["headerValue"] = headerValue;
        obj["bodyPattern"] = bodyPattern;
        obj["block"] = block;
        return obj;
    }

    static FilterRule fromJson(const QJsonObject &obj) {
        FilterRule rule;
        rule.urlPattern = obj["urlPattern"].toString();
        rule.headerKey = obj["headerKey"].toString();
        rule.headerValue = obj["headerValue"].toString();
        rule.bodyPattern = obj["bodyPattern"].toString();
        rule.block = obj["block"].toBool();
        return rule;
    }
};

namespace Ui {
class FilterDialog;
}

class FilterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FilterDialog(QWidget *parent = nullptr);
    ~FilterDialog();

    void updateRulesTable(const QList<FilterRule> &rules); // Populează tabelul
    QList<FilterRule> getRules() const;                   // Returnează regulile din tabel

signals:
    void rulesUpdated(const QList<FilterRule> &rules); // Semnal pentru reguli salvate

private slots:
    void onAddRuleClicked();
    void onEditRuleClicked();
    void onDeleteRuleClicked();
    void onSaveApplyClicked();

private:
    Ui::FilterDialog *ui;
};

#endif // FILTERDIALOG_H
