#ifndef HTTPPROXY_H
#define HTTPPROXY_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHash>
#include "httprequest.h"
#include "threadpool.h"

#include <QByteArray>

class HttpProxy : public QObject
{
    Q_OBJECT

public:
    explicit HttpProxy(int port, QObject *parent = nullptr);
    ~HttpProxy();

    bool start();
    void stop();

    QHash<QString, HttpRequest> getCache() const;

    void writeToFile(const HttpRequest &request, const QString &status);

    void forwardRequest(const HttpRequest &request);
    void forwardAllRequests();
    void handleConnect(QTcpSocket *clientSocket, const HttpRequest &request);

    QByteArray constructHttpRequest(const HttpRequest &request);

    // QHash<QString, QTcpSocket*> getClientSockets() const
    //{
    //     return clientSockets;
    // }

    HttpRequest getRequestFromCache(const QString &url) const
    {
        if (cache.contains(url)) {
            return cache.value(url);
        }
        return HttpRequest();
    }

    ThreadPool &getThreadPool() { return threadPool; }

signals:
    void logMessage(const QString &msg);


private slots:
    void onNewConnection();
    void handleClient();

private:
    QTcpServer *server;
    int port;
    QHash<QString, HttpRequest> cache;
    ThreadPool threadPool;
       QTcpSocket *clientSocket;

    void handleHttpRequest(QTcpSocket *clientSocket, const HttpRequest &request);

    void addToCache(const HttpRequest &request);
    void handleConnectionFailure(QTcpSocket *clientSocket, const QString &host, const QString &error);
    bool shouldBlockRequest(const QString &url) ;

    QStringList blockedDomains;
    void loadBlockedDomains();
    bool isDomainBlocked(const QString &url);



};

#endif // HTTPPROXY_H
