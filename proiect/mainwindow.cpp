#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTableWidget>
#include <QStringList>
#include <QString>
#include <QTextEdit>
#include <QDebug>
#include <vector>
#include <QDateTime>


// Structura pentru stocarea unui log entry


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    serverRunning(false),
    proxy(new HttpProxy(8080, this)),  // Port 8080, as an example
    process(new QProcess(this))
{
    ui->setupUi(this);

    // Configurăm tabelul pentru afișarea logurilor
    setupLogTable();

    // Conectăm semnalele pentru loguri și butoane
    connect(proxy, &HttpProxy::logMessage, this, &MainWindow::logMessage);
    connect(ui->firefoxButton, &QPushButton::clicked, this, &MainWindow::on_firefoxButton_clicked);
    //connect(ui->processLogsButton, &QPushButton::clicked, this, &MainWindow::processLogs);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Configurăm tabelul pentru afișarea logurilor
void MainWindow::setupLogTable()
{
    // Configurăm header-ul tabelului
    QStringList headers = { "Time", "Type", "Direction", "Method", "URL" };
    ui->logTableWidget->setColumnCount(headers.size());
    ui->logTableWidget->setHorizontalHeaderLabels(headers);
    ui->logTableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->logTableWidget->setSelectionBehavior(QTableWidget::SelectRows);
    ui->logTableWidget->setEditTriggers(QTableWidget::NoEditTriggers);
}

// Funcție pentru procesarea logurilor din logTextEdit


// Funcție pentru a parsa logurile dintr-un text brut
std::vector<LogEntry> MainWindow::parseLogs(const QString &logText)
{
    std::vector<LogEntry> logs;

    // Împărțim textul în linii
    QStringList lines = logText.split('\n', Qt::SkipEmptyParts);

    // Parsăm fiecare linie
    for (const QString &line : lines) {
        QStringList parts = line.split(' ', Qt::SkipEmptyParts);

        if (parts.size() < 5) {
            qDebug() << "Linie invalidă: " << line;
            continue; // Ignorăm liniile incomplete
        }

        LogEntry entry;
        entry.time = parts[0];
        entry.type = parts[1];
        entry.direction = parts[2];
        entry.method = parts[3];
        entry.url = parts.mid(4).join(" "); // Concatenăm restul ca URL

        logs.push_back(entry);
    }

    return logs;
}

// Funcție pentru afișarea logurilor în tabel
void MainWindow::displayLogsInTable(const std::vector<LogEntry> &logs)
{
    // Resetăm tabelul
    ui->logTableWidget->setRowCount(0);

    // Adăugăm fiecare log ca un rând nou în tabel
    for (const LogEntry &entry : logs) {
        int row = ui->logTableWidget->rowCount();
        ui->logTableWidget->insertRow(row);

        ui->logTableWidget->setItem(row, 0, new QTableWidgetItem(entry.time));
        ui->logTableWidget->setItem(row, 1, new QTableWidgetItem(entry.type));
        ui->logTableWidget->setItem(row, 2, new QTableWidgetItem(entry.direction));
        ui->logTableWidget->setItem(row, 3, new QTableWidgetItem(entry.method));
        ui->logTableWidget->setItem(row, 4, new QTableWidgetItem(entry.url));
    }
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
    // Analizăm mesajul pentru a extrage metoda și URL-ul
    QStringList lines = msg.split('\n', Qt::SkipEmptyParts); // Împărțim mesajul în linii
    for (const QString &line : lines) {
        // Împărțim linia pentru a detecta metoda
        QStringList parts = line.split(' ', Qt::SkipEmptyParts);
        if (parts.size() < 2) {
            continue; // Ignorăm liniile invalide
        }

        QString method = parts[0]; // Prima parte este metoda
        QString url = parts.mid(1).join(" "); // Restul este URL-ul

        // Filtrăm metodele HTTP dorite
        if (method == "CONNECT" || method == "GET" || method == "POST") {
            // Adăugăm intrarea în tabel
            int row = ui->logTableWidget->rowCount();
            ui->logTableWidget->insertRow(row);

            ui->logTableWidget->setItem(row, 0, new QTableWidgetItem(QDateTime::currentDateTime().toString())); // Timpul
            ui->logTableWidget->setItem(row, 1, new QTableWidgetItem("HTTP")); // Tipul
            ui->logTableWidget->setItem(row, 2, new QTableWidgetItem("Incoming")); // Direcția
            ui->logTableWidget->setItem(row, 3, new QTableWidgetItem(method)); // Metoda
            ui->logTableWidget->setItem(row, 4, new QTableWidgetItem(url)); // URL-ul
        }
    }
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
