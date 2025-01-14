#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include <QDebug>
#include <QMessageBox>

#include <QStyleFactory>
#include <QPropertyAnimation>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    proxy(new HttpProxy(8080, this)),
    process(new QProcess(this)),
    serverRunning(false),
    threadPool(4)
{
    ui->setupUi(this);

    qApp->setStyle(QStyleFactory::create("Fusion"));

    QFont font("Arial", 10, QFont::Bold);
    ui->logTableWidget->setFont(font);
    ui->startButton->setFont(font);
    ui->firefoxButton->setFont(font);
    ui->ForwardpushButton->setFont((font));
    ui->detailsTextEdit->setFont(font);




    setupLogTable();
    ui->detailsTextEdit->setVisible(false);

    connect(proxy, &HttpProxy::logMessage, this, &MainWindow::logMessage);

    connect(ui->firefoxButton, &QPushButton::clicked, this, &MainWindow::on_firefoxButton_clicked);
    connect(ui->logTableWidget,   &QTableWidget:: cellClicked, this, &MainWindow::onLogTableCellClicked);

    connect(ui->ForwardpushButton, &QPushButton::clicked, this, &MainWindow::on_ForwardpushButton_clicked);
}

MainWindow::~MainWindow()
{
    for (QProcess *firefoxProcess : firefoxProcesses)
    {
        if (firefoxProcess->state() == QProcess::Running)
        {
            firefoxProcess->terminate();
            if (!firefoxProcess->waitForFinished(3000))
            {
                firefoxProcess->kill();
            }
        }
        delete firefoxProcess;
    }

    delete ui;
}

void MainWindow::setupLogTable()
{
    QStringList headers={"Time", "Type", "Direction", "Method",  "URL"};

    ui->logTableWidget->setAlternatingRowColors(true);
    ui->logTableWidget->setStyleSheet(
        "QTableWidget { background-color: #ffffff; alternate-background-color: #f9f9f9; }"
        );
    ui->logTableWidget->horizontalHeader()->setStyleSheet(
        "QHeaderView::section { background-color: #4CAF50; color: white; }"
        );
    ui->logTableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->logTableWidget->verticalHeader()->setVisible(false);


    ui->logTableWidget->setColumnCount(headers.size());
    ui->logTableWidget->setHorizontalHeaderLabels(headers);
    ui->logTableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->logTableWidget->setSelectionBehavior( QTableWidget::SelectRows);
     ui->logTableWidget->setEditTriggers(QTableWidget:: NoEditTriggers );
}

void MainWindow::logMessage(const QString &msg)
{
    QStringList allowedMethods={"GET", "POST", "CONNECT"};

    if (!msg.startsWith("Incoming") && !msg.startsWith("Processed"))
    {
        qDebug() << msg;
        return;
    }

    QStringList parts=msg.split(' ', Qt::SkipEmptyParts);

    if (parts.size()<3)
    {
        return;
    }

    QString direction=parts[0];
    QString method=parts[1];
    QString url=parts.mid(2).join(" ");

    if (!allowedMethods.contains(method))
    {
        return;
    }


    QString type=url.startsWith("https://") ? "HTTPS" : "HTTP";
    int row=ui->logTableWidget->rowCount();

    ui->logTableWidget->insertRow(row);
    ui->logTableWidget->setItem(row, 0, new QTableWidgetItem(QDateTime::currentDateTime().toString()));
    ui->logTableWidget->setItem(row, 1, new QTableWidgetItem(type));
    ui->logTableWidget->setItem(row, 2, new QTableWidgetItem(direction));
    ui->logTableWidget->setItem(row, 3, new QTableWidgetItem(method));
    ui->logTableWidget->setItem(row, 4, new QTableWidgetItem(url));
}

void MainWindow::onLogTableCellClicked(int row, int column)
{
    Q_UNUSED(column);
    QTableWidgetItem *urlItem=ui->logTableWidget->item(row, 4);

    if (!urlItem)
        return;
    QString url =urlItem->text();
    QHash<QString, HttpRequest>cache=proxy->getCache();

    if (cache.contains(url) )
    {
        const HttpRequest &request=cache.value(url);
        QString details;

        details += "Method: " + request.getMethod() + "\n";
        details += "URL: " + request.getUrl() + "\n\n";
        details += "Headers:\n";

        QHash<QString, QString> headers=request.getHeaders();

        for (auto it=headers.begin(); it != headers.end(); ++it)
        {
            details += it.key() + ": " + it.value() + "\n";
        }

        details += "\nBody:\n" + QString::fromUtf8(request.getBody());
        ui->detailsTextEdit->setPlainText(details);
        ui->detailsTextEdit->setVisible(true);

    }
    else
    {
        ui->detailsTextEdit->setPlainText("Request not found in cache.");
        ui->detailsTextEdit->setVisible(true);
    }
}

void MainWindow::displayCache()
{
    ui->logTableWidget->setRowCount(0);

    QHash<QString, HttpRequest> cache=proxy->getCache();

    for (auto it=cache.begin(); it != cache.end(); ++it)
    {
        const HttpRequest &request=it.value();
        int row=ui->logTableWidget->rowCount();
        ui->logTableWidget->insertRow(row);

        QString type=request.getUrl().startsWith("https://") ? "HTTPS" : "HTTP";

        ui->logTableWidget->setItem(row, 0,new QTableWidgetItem(QDateTime::currentDateTime().toString()));
        ui->logTableWidget->setItem(row, 1,new QTableWidgetItem(type));
        ui->logTableWidget->setItem(row, 2,new QTableWidgetItem("Cached"));
        ui->logTableWidget->setItem(row, 3, new QTableWidgetItem(request.getMethod()));
        ui->logTableWidget->setItem(row, 4,new QTableWidgetItem(request.getUrl()) );
    }
}

void MainWindow::on_startButton_clicked()
{
    if (!serverRunning)
    {
        if (proxy->start()) {
            ui->startButton->setText("Stop Server");
            serverRunning=true;
        }
    }
    else
    {
        proxy->stop();
        ui->startButton->setText("Start Server");
        serverRunning=false;
    }
}

void MainWindow::on_firefoxButton_clicked()
{
    static QProcess *firefoxProcess = nullptr;

    if (firefoxProcess && firefoxProcess->state() == QProcess::Running)
    {
        qDebug() << "Firefox este deja pornit.";
        return;
    }

    firefoxProcess = new QProcess(this);

    QString firefoxPath = "/usr/bin/firefox";

    firefoxProcess->start(firefoxPath);

     firefoxProcesses.append(firefoxProcess);

    if (!firefoxProcess->waitForStarted())
    {
        qDebug() << "failed to open firefox";
        delete firefoxProcess;

        firefoxProcess = nullptr;
        return;
    }

    qDebug() << "firefox successfully opened";

    //conectez proc ca sa resetez var static cand firefox se inchide
    connect(firefoxProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,
            [=]() {
                qDebug() << "Firefox process finished.";
                firefoxProcess->deleteLater();
                firefoxProcess = nullptr;
            });
}


#include <QSslSocket>
#include <QTcpSocket>
#include <QFile>
#include <QStandardPaths>

void MainWindow::logRequestAndResponse(const QByteArray &responseData)
{
    QString desktopPath=QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QString filePath=desktopPath + "/" + "firefox.txt";
    QFile file(filePath);

    if (!file.open(QIODevice::Append | QIODevice::Text))
    {
        qDebug() << "Failed to open file on Desktop for appending:" << filePath;
        return;
    }
    QTextStream out(&file);

    out << "************************************************\n" ;
    out << "Response Data:\n" << QString(responseData) << "\n";
    out << "************************************************\n";
    file.close();

    qDebug() << "Logged response data to file:" << filePath;
}

void MainWindow::on_ForwardpushButton_clicked()
{
    int selectedRow=ui->logTableWidget->currentRow();

        if (selectedRow<0)
    {
        QMessageBox::warning(this, "error", "No request selected");
        return;
    }

    QTableWidgetItem *urlItem=ui->logTableWidget->item(selectedRow, 4);

    if (!urlItem)
    {
        QMessageBox::warning(this, "error", "Failed to retrieve the selected request.");

        return;
    }
    QString urlString=urlItem->text();
    if (!urlString.startsWith("http://") && !urlString.startsWith("https://"))
    {
        urlString="https://" + urlString;
    }
    QUrl url(urlString);

    if (!url.isValid())
    {
        qDebug() << "Invalid URL: " << urlString;
        QMessageBox::warning(this, "error", "Invalid URL: " + urlString);
        return;
    }

    QString host=url.host();
    quint16 port=url.port() == -1 ? (url.scheme() == "https" ? 443 : 80) : url.port();

    QByteArray requestData;
    QByteArray responseData;

     requestData.append("GET " + url.path().toUtf8() + " HTTP/1.1\r\n");
    requestData.append("Host: " + host.toUtf8() + "\r\n");
    requestData.append("Connection: close\r\n\r\n");

    QAbstractSocket *serverSocket=nullptr;

    if (url.scheme() == "https")
    {
        serverSocket=new QSslSocket(this);
        auto *sslSocket=qobject_cast<QSslSocket*>(serverSocket);
        sslSocket->connectToHostEncrypted(host, port);

         if (!sslSocket->waitForEncrypted(10000))
        {
            qDebug() << "SSL error: " << sslSocket->errorString();
            QMessageBox::warning(this, "error", "SSL Connection error: " + sslSocket->errorString());

            return;
        }

    }
    else
        {
        serverSocket=new QTcpSocket(this);
        serverSocket->connectToHost(host, port);

        if (!serverSocket->waitForConnected(10000))
        {
            qDebug() << "TCP Connection error: " << serverSocket->errorString();
            QMessageBox::warning(this, "error", "TCP Connection error: " + serverSocket->errorString());
            return;
        }
    }


     serverSocket->write(requestData);

    if (!serverSocket->waitForBytesWritten(10000))
     {
        //qDebug() << "error writing request: " << serverSocket->errorString();
        QMessageBox::warning(this, "error", "error writing request: " + serverSocket->errorString());
        return;
    }


    if (!serverSocket->waitForReadyRead(10000))
    {
        //qDebug() << "timeout waiting for response.";
        QMessageBox::warning(this, "error", "Timeout waiting for response");

        return;
    }

    while (serverSocket->bytesAvailable() || serverSocket->waitForReadyRead(100))
    {
        responseData.append(serverSocket->readAll());
    }

    ui->detailsTextEdit->setPlainText(QString::fromUtf8(responseData));

    logRequestAndResponse(responseData);

    ui->detailsTextEdit->setVisible(true);
    serverSocket->deleteLater();
}
