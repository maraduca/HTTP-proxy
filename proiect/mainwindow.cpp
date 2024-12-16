#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    proxy(new HttpProxy(8080, this)),
    process(new QProcess(this)),
    serverRunning(false)
{
    ui->setupUi(this);
    setupLogTable();

    ui->detailsTextEdit->setVisible(false);

    connect(proxy, &HttpProxy::logMessage, this, &MainWindow::logMessage);

    // Conectăm butonul pentru deschiderea Firefox-ului
    connect(ui->firefoxButton, &QPushButton::clicked, this, &MainWindow::on_firefoxButton_clicked);

    connect(ui->logTableWidget, &QTableWidget::cellClicked, this, &MainWindow::onLogTableCellClicked);
    //   connect(ui->openFiltersButton , &QPushButton::clicked, this, &MainWindow::on_openFilterButton_clicked);
      connect(ui->ForwardpushButton, &QPushButton::clicked, this, &MainWindow::on_ForwardpushButton_clicked);

}



MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupLogTable()
{
    QStringList headers = { "Time", "Type", "Direction", "Method", "URL" };
    ui->logTableWidget->setColumnCount(headers.size());
    ui->logTableWidget->setHorizontalHeaderLabels(headers);
    ui->logTableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->logTableWidget->setSelectionBehavior(QTableWidget::SelectRows);
    ui->logTableWidget->setEditTriggers(QTableWidget::NoEditTriggers);
}

void MainWindow::logMessage(const QString &msg)
{
    // Listează metodele HTTP relevante
    QStringList allowedMethods = { "GET", "POST", "CONNECT" };

    // Exclude mesajele care nu sunt cereri HTTP
    if (!msg.startsWith("Incoming") && !msg.startsWith("Processed")) {
        // Mesaje generale, doar le afișăm în debug și ieșim
        qDebug() << msg;
        return;
    }

    // Împarte mesajul în componente
    QStringList parts = msg.split(' ', Qt::SkipEmptyParts);
    if (parts.size() < 3) {
       // qDebug() << "Invalid log format:" << msg;
        return;
    }

    QString direction = parts[0]; // Primul cuvânt este direcția
    QString method = parts[1];    // Al doilea cuvânt este metoda
    QString url = parts.mid(2).join(" "); // Restul este URL-ul

    // Verifică dacă metoda este permisă
    if (!allowedMethods.contains(method)) {
        return; // Ignorăm logurile care nu conțin metode relevante
    }

    // Determinăm tipul (HTTP/HTTPS)
    QString type = url.startsWith("https://") ? "HTTPS" : "HTTP";

    // Adăugăm un nou rând în tabel
    int row = ui->logTableWidget->rowCount();
    ui->logTableWidget->insertRow(row);

    ui->logTableWidget->setItem(row, 0, new QTableWidgetItem(QDateTime::currentDateTime().toString())); // Timpul
    ui->logTableWidget->setItem(row, 1, new QTableWidgetItem(type));      // Tipul (HTTP/HTTPS)
    ui->logTableWidget->setItem(row, 2, new QTableWidgetItem(direction)); // Direcția (Incoming/Outgoing)
    ui->logTableWidget->setItem(row, 3, new QTableWidgetItem(method));    // Metoda
    ui->logTableWidget->setItem(row, 4, new QTableWidgetItem(url));       // URL-ul
}






void MainWindow::onLogTableCellClicked(int row, int column)
{
    Q_UNUSED(column);

    // Obține URL-ul cererii din tabel
    QTableWidgetItem *urlItem = ui->logTableWidget->item(row, 4); // Coloana URL
    if (!urlItem) return;

    QString url = urlItem->text();

    // Caută cererea în cache
    QHash<QString, HttpRequest> cache = proxy->getCache();
    if (cache.contains(url)) {
        const HttpRequest &request = cache.value(url);

        // Construiți detaliile complete ale cererii
        QString details;
        details += "Method: " + request.getMethod() + "\n";
        details += "URL: " + request.getUrl() + "\n\n";

        details += "Headers:\n";
        QHash<QString, QString> headers = request.getHeaders();
        for (auto it = headers.begin(); it != headers.end(); ++it) {
            details += it.key() + ": " + it.value() + "\n";
        }

        details += "\nBody:\n" + QString::fromUtf8(request.getBody());

        // Afișează detaliile în QTextEdit și face widget-ul vizibil
        ui->detailsTextEdit->setPlainText(details);
        ui->detailsTextEdit->setVisible(true);
    } else {
        ui->detailsTextEdit->setPlainText("Request not found in cache.");
        ui->detailsTextEdit->setVisible(true);
    }
}



void MainWindow::displayCache()
{
    ui->logTableWidget->setRowCount(0); // Resetează tabelul

    QHash<QString, HttpRequest> cache = proxy->getCache();
    for (auto it = cache.begin(); it != cache.end(); ++it) {
        const HttpRequest &request = it.value();

        int row = ui->logTableWidget->rowCount();
        ui->logTableWidget->insertRow(row);

        QString type = request.getUrl().startsWith("https://") ? "HTTPS" : "HTTP";

        ui->logTableWidget->setItem(row, 0, new QTableWidgetItem(QDateTime::currentDateTime().toString())); // Timpul
        ui->logTableWidget->setItem(row, 1, new QTableWidgetItem(type));      // Tipul (HTTP/HTTPS)
        ui->logTableWidget->setItem(row, 2, new QTableWidgetItem("Cached"));  // Direcția (Cached)
        ui->logTableWidget->setItem(row, 3, new QTableWidgetItem(request.getMethod())); // Metoda
        ui->logTableWidget->setItem(row, 4, new QTableWidgetItem(request.getUrl()));    // URL-ul
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

void MainWindow::on_firefoxButton_clicked()
{
    if (process->state() == QProcess::Running) {
        qDebug() << "Firefox is already running.";
        return;
    }

    process->start("/usr/bin/firefox");
    if (!process->waitForStarted()) {
        qDebug() << "Failed to open Firefox.";
    } else {
        qDebug() << "Mozilla Firefox opened successfully.";
    }
}


void MainWindow::on_ForwardpushButton_clicked()
{
    int selectedRow = ui->logTableWidget->currentRow();

    if (selectedRow >= 0) {
        // Forward cerere selectată
        QTableWidgetItem *urlItem = ui->logTableWidget->item(selectedRow, 4);
        if (!urlItem) {
            QMessageBox::warning(this, "Error", "Failed to retrieve the selected request.");
            return;
        }

        QString url = urlItem->text();
        QHash<QString, HttpRequest> cache = proxy->getCache();

        if (cache.contains(url)) {
            HttpRequest request = cache.value(url);
            request.debugPrint(); // Loghează detalii cerere

            // Forward cererea
            proxy->forwardRequest(request);

            // Șterge cererea din tabel și din cache
            ui->logTableWidget->removeRow(selectedRow);
            cache.remove(url); // Asigură-te că cererea este ștearsă

            emit logMessage("Forwarded request: " + url);
        } else {
            QMessageBox::warning(this, "Error", "Request not found in cache.");
        }
    } else {
        // Forward toate cererile din cache
        proxy->forwardAllRequests();

        // Golește tabelul
        ui->logTableWidget->setRowCount(0);

        emit logMessage("Forwarded all requests.");
    }
}


