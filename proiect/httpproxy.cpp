#include "httpproxy.h"
#include <QHostAddress>
#include <QDebug>

HttpProxy::HttpProxy(int port, QObject *parent)
    : QObject(parent), port(port), server(new QTcpServer(this))
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
    // Get the client socket from the sender object (QTcpSocket that triggered readyRead)
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket *>(sender());
    if (!clientSocket) return;

    // Read the client request data
    QByteArray requestData = clientSocket->readAll();
    QString request = QString::fromUtf8(requestData);  // Interpret as UTF-8
    emit logMessage("Request received:\n" + request);

    // Parse HTTP request
    QTextStream requestStream(&request);
    QString method, url;
    QString line;
    QString headers;

    requestStream >> method >> url;
    requestStream.readLine();  // Skip the rest of the request line

    while (requestStream.readLineInto(&line) && !line.isEmpty()) {
        headers += line + "\r\n";
    }

    // Handle HTTPS tunneling (CONNECT method)
    if (method == "CONNECT") {
        // Parse host and port
        QString host = url.section(':', 0, 0);
        QString port = url.section(':', 1, 1);
        if (port.isEmpty()) port = "443";  // Default to port 443 for HTTPS

        // Connect to the target server
        QTcpSocket *serverSocket = new QTcpSocket(this);
        connect(serverSocket, &QTcpSocket::readyRead, [=]() {
            QByteArray serverResponse = serverSocket->readAll();
            clientSocket->write(serverResponse);  // Directly forward response to client
        });
        connect(serverSocket, &QTcpSocket::disconnected, serverSocket, &QTcpSocket::deleteLater);

        serverSocket->connectToHost(host, port.toUShort());
        if (!serverSocket->waitForConnected(5000)) {
            emit logMessage("Connection to " + host + " failed: " + serverSocket->errorString());
            clientSocket->disconnectFromHost();
            return;
        }

        // Send HTTP 200 Connection Established response to client
        clientSocket->write("HTTP/1.1 200 Connection Established\r\n\r\n");
        connect(clientSocket, &QTcpSocket::readyRead, [=]() {
            QByteArray clientData = clientSocket->readAll();
            serverSocket->write(clientData);
        });

    } else {
        // Handle standard HTTP requests
        QString host = url.mid(url.indexOf("://") + 3);
        QString path = "/";
        int pathIndex = host.indexOf('/');
        if (pathIndex != -1) {
            path = host.mid(pathIndex);
            host = host.left(pathIndex);
        }

        // Create a new socket to connect to the destination server
        QTcpSocket *serverSocket = new QTcpSocket(this);
        connect(serverSocket, &QTcpSocket::readyRead, [=]() {
            QByteArray serverResponse = serverSocket->readAll();
            clientSocket->write(serverResponse);  // Directly forward response to client
        });
        connect(serverSocket, &QTcpSocket::disconnected, serverSocket, &QTcpSocket::deleteLater);

        serverSocket->connectToHost(host, 80);  // Default to port 80 for HTTP
        if (!serverSocket->waitForConnected(5000)) {
            emit logMessage("Connection to " + host + " failed: " + serverSocket->errorString());
            clientSocket->disconnectFromHost();
            return;
        }

        // Build and send the HTTP request to the destination server
        QString fullRequest = method + " " + path + " HTTP/1.1\r\n" +
                              "Host: " + host + "\r\n" +
                              "Connection: close\r\n" +
                              headers + "\r\n";

        serverSocket->write(fullRequest.toUtf8());

        // Close the client socket when the request is complete
        connect(clientSocket, &QTcpSocket::disconnected, clientSocket, &QTcpSocket::deleteLater);
    }
}

void HttpProxy::handleConnect(QTcpSocket *clientSocket, QTcpSocket *serverSocket)
{
    // Forward data from client to server
    connect(clientSocket, &QTcpSocket::readyRead, this, [=]() {
        QByteArray clientData = clientSocket->readAll();
        serverSocket->write(clientData);  // Send client's data to the server
    });

    // Forward data from server to client
    connect(serverSocket, &QTcpSocket::readyRead, this, [=]() {
        QByteArray serverData = serverSocket->readAll();
        clientSocket->write(serverData);  // Send server's data to the client
    });

    // Handle disconnection of either socket by closing both
    auto closeSockets = [=]() {
        clientSocket->close();
        serverSocket->close();
        clientSocket->deleteLater();
        serverSocket->deleteLater();
    };

    connect(clientSocket, &QTcpSocket::disconnected, this, closeSockets);
    connect(serverSocket, &QTcpSocket::disconnected, this, closeSockets);
}
