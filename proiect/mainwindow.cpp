#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    proxy(new HttpProxy(8080, this)),
    process(new QProcess(this)),
    serverRunning(false)
{
    ui->setupUi(this);

    loadRulesFromFile("rules.json");


    proxy->setFilterRules(filterRules);
    setupLogTable();

      ui->detailsTextEdit->setVisible(false);

    // Conectăm logurile proxy-ului la tabelul interfeței
    connect(proxy, &HttpProxy::logMessage, this, &MainWindow::logMessage);

    // Conectăm butonul pentru deschiderea Firefox-ului
    connect(ui->firefoxButton, &QPushButton::clicked, this, &MainWindow::on_firefoxButton_clicked);

    connect(ui->logTableWidget, &QTableWidget::cellClicked, this, &MainWindow::onLogTableCellClicked);
 //   connect(ui->openFiltersButton , &QPushButton::clicked, this, &MainWindow::on_openFilterButton_clicked);
    connect(ui->openFiltersButton, &QPushButton::clicked, this, &MainWindow::on_openFiltersButton_clicked);

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

    // Împarte mesajul în componente
    QStringList parts = msg.split(' ', Qt::SkipEmptyParts);
    if (parts.size() < 3) {
        qDebug() << "Invalid log format:" << msg;
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
    ui->logTableWidget->setItem(row, 3, new QTableWidgetItem(method));    // Metoda (GET, POST, CONNECT)
    ui->logTableWidget->setItem(row, 4, new QTableWidgetItem(url));       // URL-ul
}



void MainWindow::applyFilter(const QString &method, const QString &filterText)
{
    for (int row = 0; row < ui->logTableWidget->rowCount(); ++row) {
        bool match = false;

        // Filtrare pe baza metodei
        if (!method.isEmpty()) {
            QTableWidgetItem *methodItem = ui->logTableWidget->item(row, 3); // Coloana Metoda
            if (methodItem && methodItem->text() == method) {
                match = true;
            }
        }

        // Filtrare pe baza textului (de exemplu, URL sau alte câmpuri)
        if (!filterText.isEmpty()) {
            QTableWidgetItem *urlItem = ui->logTableWidget->item(row, 4); // Coloana URL
            if (urlItem && urlItem->text().contains(filterText, Qt::CaseInsensitive)) {
                match = true;
            }
        }

        // Ascunde rândul dacă nu se potrivește
        ui->logTableWidget->setRowHidden(row, !match);
    }
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


void MainWindow::saveRulesToFile(const QString &filePath) {
    QJsonArray rulesArray;
    for (const FilterRule &rule : filterRules) {
        rulesArray.append(rule.toJson());
    }

    QJsonDocument doc(rulesArray);
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
    }
}

void MainWindow::loadRulesFromFile(const QString &filePath) {
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QJsonArray rulesArray = doc.array();

        filterRules.clear();
        for (const auto &value : rulesArray) {
            filterRules.append(FilterRule::fromJson(value.toObject()));
        }
    }
}

void MainWindow::on_openFiltersButton_clicked() {
    FilterDialog dialog(this);

    // Populează tabelul din dialog cu regulile existente
    dialog.updateRulesTable(filterRules);

    // Conectează semnalul rulesUpdated la funcția pentru actualizare
    connect(&dialog, &FilterDialog::rulesUpdated, this, [this](const QList<FilterRule> &rules) {
        filterRules = rules;                // Actualizează lista de reguli
        saveRulesToFile("rules.json");      // Salvează regulile în fișier
        proxy->setFilterRules(filterRules); // Trimite regulile la proxy
    });

    // Deschide dialogul și așteaptă
    dialog.exec();
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
    process->start("/usr/bin/firefox");
    if (!process->waitForStarted()) {
        qDebug() << "Failed to open Firefox.";
    } else {
        qDebug() << "Mozilla Firefox opened successfully.";
    }
}
