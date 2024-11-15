#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    serverRunning(false),
    proxy(new HttpProxy(8080, this)),  // Port 8080, as an example
    process(new QProcess(this))
{
    ui->setupUi(this);
    connect(proxy, &HttpProxy::logMessage, this, &MainWindow::logMessage);
    connect(ui->firefoxButton, &QPushButton::clicked, this, &MainWindow::on_firefoxButton_clicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_startButton_clicked()
{
    if (!serverRunning) {
        if (proxy->start()) {
            ui->startButton->setText("Stop Server");
            serverRunning = true;
        }
    } else {
        proxy->stop();
        ui->startButton->setText("Start Server");
        serverRunning = false;
    }
}

void MainWindow::logMessage(const QString &msg)
{
    ui->logTextEdit->append(msg);
}

void MainWindow::on_firefoxButton_clicked()
{
    // Launch Firefox
    process->start("/usr/bin/firefox");

    // Optional: Check if it starts successfully
    if (!process->waitForStarted()) {
        qDebug()<<"Failed to open Firefox.";
    } else {
        qDebug()<<"Mozilla Firefox opened successfully.";
    }
}

