#include "httpproxy.h"
#include <QHostAddress>
#include <QDebug>

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

void HttpProxy::handleHttpRequest(QTcpSocket *clientSocket, const HttpRequest &request)
{
    QString url = request.getUrl();

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
