#include "httpproxy.h"
#include <QHostAddress>
#include <QDebug>
#include <QRegularExpression>

HttpProxy::HttpProxy(int port, QObject *parent)
    : QObject(parent), server(new QTcpServer(this)), port(port)
{
    connect(server, &QTcpServer::newConnection, this, &HttpProxy::onNewConnection);
}

HttpProxy::~HttpProxy()
{
    stop();
}

bool HttpProxy::start()
{
    if (!server->listen(QHostAddress::Any, port)) {
        emit logMessage("Server failed to start: " + server->errorString());
        return false;
    }
    emit logMessage("Server listening on port " + QString::number(port));
    return true;
}

bool HttpProxy::shouldBlockRequest(const QString &url, const QHash<QString, QString> &headers) {
    for (const FilterRule &rule : filterRules) {
        // Verifică URL Pattern
        if (!rule.urlPattern.isEmpty() && url.contains(QRegularExpression(rule.urlPattern))) {
            // Verifică Header Key și Value (dacă există)
            if (!rule.headerKey.isEmpty()) {
                if (headers.contains(rule.headerKey)) {
                    if (rule.headerValue.isEmpty() || headers.value(rule.headerKey).contains(rule.headerValue)) {
                        return rule.block; // Blocăm sau permitem cererea, conform regulii
                    }
                }
            } else {
                // Doar URL-ul se potrivește, decidem pe baza flag-ului `block`
                return rule.block;
            }
        }
    }

    return false; // Dacă nicio regulă nu se potrivește, nu blocăm cererea
}


void HttpProxy::stop()
{
    server->close();
    emit logMessage("Server stopped.");
}

void HttpProxy::onNewConnection()
{
    QTcpSocket *clientSocket = server->nextPendingConnection();
    emit logMessage("New client connection established.");
    connect(clientSocket, &QTcpSocket::readyRead, this, &HttpProxy::handleClient);
    connect(clientSocket, &QTcpSocket::disconnected, clientSocket, &QTcpSocket::deleteLater);
}

void HttpProxy::handleClient()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket *>(sender());
    if (!clientSocket) return;

    QByteArray requestData = clientSocket->readAll();
    HttpRequest request = HttpRequest::parse(requestData);

    if (request.getMethod().isEmpty() || request.getUrl().isEmpty()) {
        emit logMessage("Invalid HTTP request received.");
        clientSocket->disconnectFromHost();
        return;
    }

    emit logMessage("Incoming " + request.getMethod() + " " + request.getUrl());
    handleHttpRequest(clientSocket, request);
}

void HttpProxy::setFilterRules(const QList<FilterRule> &rules) {
    filterRules = rules;
}


void HttpProxy::handleHttpRequest(QTcpSocket *clientSocket, const HttpRequest &request)
{
    QString url = request.getUrl();

    // if (shouldBlockRequest(url, this.)) {
    //     emit logMessage("Blocked request to: " + url);
    //     clientSocket->write("HTTP/1.1 403 Forbidden\r\nContent-Length: 0\r\n\r\n");
    //     clientSocket->disconnectFromHost();
    //     return;
    // }

    // Verifică dacă cererea este deja în cache
    if (cache.contains(url)) {
        emit logMessage("Request already cached: " + url);
        return;
    }

    // Salvează cererea în cache
    emit logMessage("Caching request: " + url);
    cache.insert(url, request);

    // Închide conexiunea clientului fără a trimite nimic
    clientSocket->disconnectFromHost();
}

void HttpProxy::handleConnectionFailure(QTcpSocket *clientSocket, const QString &host, const QString &error)
{
    emit logMessage("Connection to " + host + " failed: " + error);
    clientSocket->disconnectFromHost();
}

QHash<QString, HttpRequest> HttpProxy::getCache() const
{
    return cache;
}
