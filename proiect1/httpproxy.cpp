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
    : QObject(parent), server(new QTcpServer(this)), port(port), threadPool(5)
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

void HttpProxy::onNewConnection() {
    clientSocket = server->nextPendingConnection();
    emit logMessage("New client connection established.");

    // Conectează semnalele socket-ului clientului
    connect(clientSocket, &QTcpSocket::readyRead, this, &HttpProxy::handleClient);
    connect(clientSocket, &QTcpSocket::disconnected, clientSocket, &QTcpSocket::deleteLater);

    // Resetăm referința la clientSocket când clientul se deconectează
    connect(clientSocket, &QTcpSocket::disconnected, [this]() {
        clientSocket = nullptr;
        emit logMessage("Client disconnected.");
    });
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

    // Adăugăm verificarea pentru cererile CONNECT (HTTPS)
    // if (request.getMethod() == "CONNECT")
    // {
    //     emit logMessage("Handling CONNECT request for " + request.getUrl());
    //     handleConnect(clientSocket, request); // Gestionează tunelarea
    //     return;
    // }

    // Continuăm procesarea pentru cererile HTTP normale
    emit logMessage("Incoming " + request.getMethod() + " " + request.getUrl());
    handleHttpRequest(clientSocket, request);
}


void HttpProxy::handleConnect(QTcpSocket *clientSocket, const HttpRequest &request) {
    // Extrage host-ul și portul din URL-ul CONNECT
    QString host = request.getUrl().section(':', 0, 0);
    quint16 port = request.getUrl().section(':', 1, 1).toUShort();

    // Setează portul implicit dacă nu este specificat
    if (port == 0) {
        port = 443; // Port implicit pentru HTTPS
    }

    qDebug() << "Handling CONNECT request:";
    qDebug() << "Host:" << host;
    qDebug() << "Port:" << port;

    QTcpSocket *serverSocket = new QTcpSocket(this);

    // Conectează socket-ul către serverul țintă
    serverSocket->connectToHost(host, port);
    if (!serverSocket->waitForConnected(3000)) {
        qDebug() << "Failed to connect to host:" << host << "on port:" << port;
        emit logMessage("Failed to connect to " + host + ":" + QString::number(port));
        clientSocket->write("HTTP/1.1 502 Bad Gateway\r\n\r\n");
        clientSocket->disconnectFromHost();
        serverSocket->deleteLater();
        return;
    }

    qDebug() << "Successfully connected to host:" << host << "on port:" << port;

    // Trimite răspunsul către browser: tunelul este gata
    clientSocket->write("HTTP/1.1 200 Connection Established\r\n\r\n");
    clientSocket->flush();

    QObject::connect(clientSocket, &QTcpSocket::readyRead, [serverSocket, clientSocket]() {
        QByteArray clientData = clientSocket->readAll();
        if (!clientData.isEmpty()) {
            serverSocket->write(clientData);
            serverSocket->flush();
        }
    });

    QObject::connect(serverSocket, &QTcpSocket::readyRead, [serverSocket, clientSocket]() {
        QByteArray serverData = serverSocket->readAll();
        if (!serverData.isEmpty()) {
            clientSocket->write(serverData);
            clientSocket->flush();
        }
    });



    // Închide conexiunile când oricare parte se deconectează
    QObject::connect(clientSocket, &QTcpSocket::disconnected, serverSocket, &QTcpSocket::disconnectFromHost);
    QObject::connect(serverSocket, &QTcpSocket::disconnected, clientSocket, &QTcpSocket::disconnectFromHost);

    // Gestionează erorile de conexiune
    // connect(serverSocket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred),
    //         [clientSocket, serverSocket](QAbstractSocket::SocketError socketError) {
    //             qDebug() << "Error occurred with target server:" << socketError;
    //             clientSocket->write("HTTP/1.1 502 Bad Gateway\r\n\r\n");
    //             clientSocket->disconnectFromHost();
    //             serverSocket->deleteLater();
    //         });
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

void HttpProxy::forwardRequest(const HttpRequest &request) {
    threadPool.addTask([this, request]() {
        if (!clientSocket) {
            qDebug() << "No client socket available.";
            return;
        }

        QString url = request.getUrl();
        QString host = url.section('/', 2, 2).section(':', 0, 0);
        quint16 port = url.startsWith("https://") ? 443 : 80;

        QTcpSocket serverSocket;
        qDebug() << "Forwarding request to:" << host << "on port:" << port;

        // Conectare la serverul țintă
        serverSocket.connectToHost(host, port);
        if (!serverSocket.waitForConnected(3000)) {
            qDebug() << "Failed to connect to host:" << host;
            clientSocket->write("HTTP/1.1 502 Bad Gateway\r\n\r\n");
            //clientSocket->disconnectFromHost();
            return;
        }

        // Trimite cererea către serverul țintă
        serverSocket.write(request.toRawRequest());
        if (!serverSocket.waitForBytesWritten(3000)) {
            qDebug() << "Failed to send request to:" << host;
            clientSocket->write("HTTP/1.1 502 Bad Gateway\r\n\r\n");
            //clientSocket->disconnectFromHost();
            return;
        }

        // Citește răspunsul de la server și transmite-l browserului
        QByteArray response;
        while (serverSocket.waitForReadyRead(5000)) {
            response += serverSocket.readAll();
        }

        if (!response.isEmpty()) {
            qDebug() << "Forwarding response to client.";
            clientSocket->write(response);
            clientSocket->waitForBytesWritten();
        } else {
            qDebug() << "No response from host:" << host;
            clientSocket->write("HTTP/1.1 502 Bad Gateway\r\n\r\n");
        }

       // clientSocket->disconnectFromHost();
       // serverSocket.close();
    });

    emit logMessage("Task added to Thread Pool for request: " + request.getUrl());
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
