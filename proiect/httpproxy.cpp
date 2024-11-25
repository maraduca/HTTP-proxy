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

    if (cache.contains(url)) {
        HttpRequest cachedRequest = cache.value(url);
        clientSocket->write(cachedRequest.getBody());
        emit logMessage("Served from cache: " + url);
        return;
    }

    QString host = url.mid(url.indexOf("://") + 3);
    QString path = "/";
    int pathIndex = host.indexOf('/');
    if (pathIndex != -1) {
        path = host.mid(pathIndex);
        host = host.left(pathIndex);
    }

    QTcpSocket *serverSocket = new QTcpSocket(this);
    connect(serverSocket, &QTcpSocket::readyRead, [=]() {
        QByteArray serverResponse = serverSocket->readAll();
        clientSocket->write(serverResponse);

        HttpRequest cachedRequest = request;
        cachedRequest.setBody(serverResponse);
        cache.insert(url, cachedRequest);

        emit logMessage("Response cached for URL: " + url + " (" + QString::number(serverResponse.size()) + " bytes)");
    });

    connect(serverSocket, &QTcpSocket::disconnected, serverSocket, &QTcpSocket::deleteLater);

    serverSocket->connectToHost(host, 80);
    if (!serverSocket->waitForConnected(5000)) {
        handleConnectionFailure(clientSocket, host, serverSocket->errorString());
        return;
    }

    QString fullRequest = request.getMethod() + " " + path + " HTTP/1.1\r\n" +
                          "Host: " + host + "\r\n" +
                          "Connection: close\r\n" +
                          request.headersToRaw() + "\r\n";

    serverSocket->write(fullRequest.toUtf8());
    emit logMessage("Forwarded request: " + request.getMethod() + " " + url);
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
