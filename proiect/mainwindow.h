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

    void on_openFilterButton_clicked(); // Slot pentru butonul de filtre
    void  applyFilter(const QString &criterion, const QString &filterText);


private:
    Ui::MainWindow *ui;
    HttpProxy *proxy;
    QProcess *process;
    bool serverRunning;

    void setupLogTable();
};

#endif // MAINWINDOW_H
