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
    serverRunning(false),
    threadPool(4)
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
     if (process->state() == QProcess::Running) {
            process->terminate(); // Încearcă să închidă browserul grațios
            if (!process->waitForFinished(3000)) { // Așteaptă 3 secunde pentru a se închide
                process->kill(); // Forțează închiderea dacă nu răspunde
            }
        }
        delete process;
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


    QTableWidgetItem *urlItem = ui->logTableWidget->item(row, 4); // Coloana URL
    if (!urlItem) return;

    QString url = urlItem->text();

    QHash<QString, HttpRequest> cache = proxy->getCache();
    if (cache.contains(url)) {
        const HttpRequest &request = cache.value(url);

        QString details;
        details += "Method: " + request.getMethod() + "\n";
        details += "URL: " + request.getUrl() + "\n\n";

        details += "Headers:\n";
        QHash<QString, QString> headers = request.getHeaders();
        for (auto it = headers.begin(); it != headers.end(); ++it) {
            details += it.key() + ": " + it.value() + "\n";
        }

        details += "\nBody:\n" + QString::fromUtf8(request.getBody());

        ui->detailsTextEdit->setPlainText(details);
        ui->detailsTextEdit->setVisible(true);
    } else {
        ui->detailsTextEdit->setPlainText("Request not found in cache.");
        ui->detailsTextEdit->setVisible(true);
    }
}



void MainWindow::displayCache()
{
    ui->logTableWidget->setRowCount(0);
    QHash<QString, HttpRequest> cache = proxy->getCache();
    for (auto it = cache.begin(); it != cache.end(); ++it) {
        const HttpRequest &request = it.value();

        int row = ui->logTableWidget->rowCount();
        ui->logTableWidget->insertRow(row);

        QString type = request.getUrl().startsWith("https://") ? "HTTPS" : "HTTP";

        ui->logTableWidget->setItem(row, 0, new QTableWidgetItem(QDateTime::currentDateTime().toString()));
        ui->logTableWidget->setItem(row, 1, new QTableWidgetItem(type));
        ui->logTableWidget->setItem(row, 2, new QTableWidgetItem("Cached"));
        ui->logTableWidget->setItem(row, 3, new QTableWidgetItem(request.getMethod()));
        ui->logTableWidget->setItem(row, 4, new QTableWidgetItem(request.getUrl()));
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
    threadPool.addTask([]() {
        QProcess process;
        QString firefoxPath = "/usr/bin/firefox"; // Calea către Firefox
        QStringList arguments;

        process.start(firefoxPath, arguments);
        if (!process.waitForStarted()) {
            qDebug() << "Failed to start Firefox.";
            return;
        }

        qDebug() << "Firefox instance started.";
        process.waitForFinished(-1); // Așteaptă până când procesul se termină
    });

    qDebug() << "Task added to ThreadPool for Firefox launch.";
}

#include <QSslSocket>
#include <QTcpSocket>
#include <QFile>
#include <QStandardPaths>

void MainWindow::logRequestAndResponse(const QByteArray &responseData) {
    // Obține calea către Desktop
    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);



    // Creează calea completă a fișierului
    QString filePath = desktopPath + "/" + "firefox.txt";

    // Creează fișierul
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Failed to create file on Desktop:" << filePath;
        return;
    }

    // Scrie conținutul în fișier
    QTextStream out(&file);
    out << "************************************************\n";
    out << "Response Data:\n" << QString(responseData) << "\n";
    out << "************************************************\n";
    file.close();

    qDebug() << "File created on Desktop:" << filePath;

}

void MainWindow::on_ForwardpushButton_clicked() {
    int selectedRow = ui->logTableWidget->currentRow();

    if (selectedRow < 0) {
        QMessageBox::warning(this, "Error", "No request selected.");
        return;
    }

    QTableWidgetItem *urlItem = ui->logTableWidget->item(selectedRow, 4);
    if (!urlItem) {
        QMessageBox::warning(this, "Error", "Failed to retrieve the selected request.");
        return;
    }

    QString urlString = urlItem->text();
    if (!urlString.startsWith("http://") && !urlString.startsWith("https://")) {
        urlString = "https://" + urlString; // Default to HTTPS
    }

    QUrl url(urlString);
    if (!url.isValid()) {
        qDebug() << "Invalid URL: " << urlString;
        QMessageBox::warning(this, "Error", "Invalid URL: " + urlString);
        return;
    }

    QString host = url.host();
    quint16 port = url.port() == -1 ? (url.scheme() == "https" ? 443 : 80) : url.port();
    QByteArray requestData;
    QByteArray responseData;

    // Example HTTP request
    requestData.append("GET " + url.path().toUtf8() + " HTTP/1.1\r\n");
    requestData.append("Host: " + host.toUtf8() + "\r\n");
    requestData.append("Connection: close\r\n\r\n");

    QAbstractSocket *serverSocket = nullptr;

    if (url.scheme() == "https") {
        // HTTPS Request
        serverSocket = new QSslSocket(this);
        auto *sslSocket = qobject_cast<QSslSocket *>(serverSocket);
        sslSocket->connectToHostEncrypted(host, port);

        if (!sslSocket->waitForEncrypted(10000)) {
            qDebug() << "SSL Connection Error: " << sslSocket->errorString();
            QMessageBox::warning(this, "Error", "SSL Connection Error: " + sslSocket->errorString());
            return;
        }
    } else {
        // HTTP Request
        serverSocket = new QTcpSocket(this);
        serverSocket->connectToHost(host, port);

        if (!serverSocket->waitForConnected(10000)) {
            qDebug() << "TCP Connection Error: " << serverSocket->errorString();
            QMessageBox::warning(this, "Error", "TCP Connection Error: " + serverSocket->errorString());
            return;
        }
    }

    serverSocket->write(requestData);
    if (!serverSocket->waitForBytesWritten(10000)) {
        qDebug() << "Error writing request: " << serverSocket->errorString();
        QMessageBox::warning(this, "Error", "Error writing request: " + serverSocket->errorString());
        return;
    }

    if (!serverSocket->waitForReadyRead(10000)) {
        qDebug() << "Timeout waiting for response.";
        QMessageBox::warning(this, "Error", "Timeout waiting for response.");
        return;
    }

    while (serverSocket->bytesAvailable() || serverSocket->waitForReadyRead(100)) {
        responseData.append(serverSocket->readAll());
    }

    // Display the response in the details text edit
    ui->detailsTextEdit->setPlainText(QString::fromUtf8(responseData));
    logRequestAndResponse(responseData);
    ui->detailsTextEdit->setVisible(true);

    serverSocket->deleteLater();
}
