#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include "httpproxy.h"
#include "threadpool.h"
#include <QMap>

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
    void on_ForwardpushButton_clicked();
    void logRequestAndResponse(const QByteArray &responseData);


private:
    Ui::MainWindow *ui;
    HttpProxy *proxy;
    QProcess *process;
    bool serverRunning;

     QList<QProcess*> firefoxProcesses;

    void setupLogTable();

    ThreadPool threadPool;
};

#endif // MAINWINDOW_H
