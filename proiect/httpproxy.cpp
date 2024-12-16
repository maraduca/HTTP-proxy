#include "httpproxy.h"
#include <QHostAddress>
#include <QDebug>
#include <QRegularExpression>
#include <fcntl.h>
#include <unistd.h>
#include <QDir>
#include <QDateTime>
#include <QMessageBox>

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
    qDebug() << "Attempting to start server on port:" << port;
    if (!server->listen(QHostAddress::Any, port))
    {
        qDebug() << "Failed to start server:" << server->errorString();
        emit logMessage("Server failed to start: " + server->errorString());
        return false;
    }
    qDebug() << "Server started successfully on port:" << port;

    emit logMessage("Server listening on port " + QString::number(port));
    return true;
}

bool HttpProxy::shouldBlockRequest(const QString &url)
{
    QStringList blockedExtensions = {".css", ".js", ".png", ".jpg", ".gif", ".woff", ".svg", ".ico", ".map"};
    QStringList blockedDomains = {"ads.", "analytics.", "track.", "doubleclick.net"};

    for (const QString &ext : blockedExtensions)
    {
        if (url.endsWith(ext))
        {
            return true;
        }
    }

    for (const QString &domain : blockedDomains)
    {
        if (url.contains(domain)) {
            return true;
        }
    }

    return false;
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

    if (request.getMethod().isEmpty() || request.getUrl().isEmpty())
    {
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

    if (shouldBlockRequest(url)) {
        emit logMessage("Filtered out: " + url);
        clientSocket->write("HTTP/1.1 204 No Content\r\nContent-Length: 0\r\n\r\n");
        clientSocket->disconnectFromHost();
        return;
    }

    if (!cache.contains(url))
    {
        cache.insert(url, request);
        emit logMessage("Cached request: " + url);
    }

    emit logMessage("Processed: " + url);
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

void HttpProxy::writeToFile(const HttpRequest &request, const QString &status)
{
    QString dirPath = "./logs";
    QDir dir(dirPath);
    if (!dir.exists() && !dir.mkpath("."))
    {
        qDebug() << "Failed to create logs directory!";
        return;
    }

    QString fileName = dirPath + "/" + QDateTime::currentDateTime().toString("yyyy-MM-dd") + ".log";
    int fd = open(fileName.toUtf8().constData(), O_WRONLY | O_CREAT | O_APPEND, 0644);

    if (fd < 0)
    {
        perror("Failed to open log file");
        return;
    }

    QByteArray data;
    data.append("Method: ").append(request.getMethod().toUtf8()).append("\n");
    data.append("URL: ").append(request.getUrl().toUtf8()).append("\n\n");

    const auto &headers = request.getHeaders();
    if (headers.isEmpty())
    {
        data.append("Headers: None\n");
    }
        else
    {
        data.append("Headers:\n");
        for (auto it = headers.begin(); it != headers.end(); ++it)
        {
            data.append(it.key().toUtf8()).append(": ").append(it.value().toUtf8()).append("\n");
        }
    }

    data.append("\nBody:\n").append(request.getBody());

    ssize_t bytesWritten = write(fd, data.constData(), data.size());
    if (bytesWritten < 0)
    {
        perror("Failed to write to log file");
    }
    else
    {
        qDebug() << "Successfully wrote" << bytesWritten << "bytes to log file.";
    }

    close(fd);
}

void HttpProxy::forwardRequest(const HttpRequest &request)
{
    QString url = request.getUrl();
    QString host;
    quint16 port = 80;

    if (url.startsWith("http://") || url.startsWith("https://"))
    {
        host = url.section('/', 2, 2).section(':', 0, 0);
        port = (url.contains(':')) ? url.section(':', -1).toUShort() : (url.startsWith("https://") ? 443 : 80);
    }
    else
    {
        host = url.section(':', 0, 0);
        port = url.section(':', 1, 1).toUShort();
    }

    if (host.isEmpty())
    {
        emit logMessage("Failed to forward request: Invalid host.");
        return;
    }

    QTcpSocket *serverSocket = new QTcpSocket(this);
    serverSocket->connectToHost(host, port);

    if (!serverSocket->waitForConnected(5000))
    {
        emit logMessage("Failed to forward request to: " + host);
        serverSocket->deleteLater();
        return;
    }

    serverSocket->write(request.toRawRequest());
    serverSocket->waitForBytesWritten();

    if (serverSocket->waitForReadyRead(5000))
    {
        QByteArray response = serverSocket->readAll();
        emit logMessage("Forwarded response from: " + host);
    }

    serverSocket->deleteLater();
    writeToFile(request, "forwarded");
    cache.remove(url);
    emit logMessage("Request removed from cache: " + url);
}

void HttpProxy::forwardAllRequests()
{
    for (auto it = cache.begin(); it != cache.end(); )
    {
        forwardRequest(it.value());
        it = cache.erase(it);
    }

    emit logMessage("All requests have been forwarded.");
}
