#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include "httpproxy.h"
#include "filterdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void logMessage(const QString &msg);
    void displayCache();
    void on_startButton_clicked();
    void on_firefoxButton_clicked();
    void onLogTableCellClicked(int row, int column);

    void  applyFilter(const QString &criterion, const QString &filterText);

     void on_openFiltersButton_clicked();


private:
    Ui::MainWindow *ui;
    HttpProxy *proxy;
    QProcess *process;
    bool serverRunning;

    void setupLogTable();
    QList<FilterRule> filterRules;

    void saveRulesToFile(const QString &filePath);
    void loadRulesFromFile(const QString &filePath);
};

#endif // MAINWINDOW_H
