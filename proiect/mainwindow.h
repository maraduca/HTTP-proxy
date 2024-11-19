#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "httpproxy.h"
#include <QProcess>
#include <vector>
#include <QString>

namespace Ui {
class MainWindow;
}

// Structura pentru a stoca un log entry
struct LogEntry {
    QString time;
    QString type;
    QString direction;
    QString method;
    QString url;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_startButton_clicked();                // Pornește/Oprește serverul
    void logMessage(const QString &msg);          // Afișează un mesaj de log în interfață
    void setupLogTable();                         // Configurează tabelul pentru loguri
    std::vector<LogEntry> parseLogs(const QString &logText); // Parsează logurile brute
    void displayLogsInTable(const std::vector<LogEntry> &logs); // Afișează logurile în tabel
    void on_firefoxButton_clicked();              // Deschide browser-ul Firefox

private:
    Ui::MainWindow *ui;                           // Interfața grafică
    HttpProxy *proxy;                             // Obiectul care gestionează proxy-ul HTTP
    bool serverRunning;                           // Flag pentru statusul serverului
    QProcess *process;                            // Proces pentru lansarea aplicațiilor externe
};

#endif // MAINWINDOW_H
