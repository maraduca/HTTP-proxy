#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "httpproxy.h"
#include <QProcess>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_startButton_clicked();
    void logMessage(const QString &msg);

    void on_firefoxButton_clicked();

private:
    Ui::MainWindow *ui;
    HttpProxy *proxy;
    bool serverRunning;
    QProcess *process;
};

#endif // MAINWINDOW_H
