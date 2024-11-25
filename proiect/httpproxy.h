#ifndef HTTPPROXY_H
#define HTTPPROXY_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHash>
#include "httprequest.h"

class HttpProxy : public QObject
{
    Q_OBJECT

public:
    explicit HttpProxy(int port, QObject *parent = nullptr);
    ~HttpProxy();

    bool start();
    void stop();

    QHash<QString, HttpRequest> getCache() const;

signals:
    void logMessage(const QString &msg);

private slots:
    void onNewConnection();
    void handleClient();

private:
    QTcpServer *server;
    int port;
    QHash<QString, HttpRequest> cache;

    void handleHttpRequest(QTcpSocket *clientSocket, const HttpRequest &request);
    void addToCache(const HttpRequest &request);
    void handleConnectionFailure(QTcpSocket *clientSocket, const QString &host, const QString &error);
};

#endif // HTTPPROXY_H
