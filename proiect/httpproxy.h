#ifndef HTTPPROXY_H
#define HTTPPROXY_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHash>
#include "httprequest.h"
#include "filterdialog.h"

class HttpProxy : public QObject
{
    Q_OBJECT

public:
    explicit HttpProxy(int port, QObject *parent = nullptr);
    ~HttpProxy();

    bool start();
    void stop();

    QHash<QString, HttpRequest> getCache() const;
         void setFilterRules(const QList<FilterRule> &rules);

signals:
    void logMessage(const QString &msg);

private slots:
    void onNewConnection();
    void handleClient();

private:
    QTcpServer *server;
    int port;
    QHash<QString, HttpRequest> cache;
     QList<FilterRule> filterRules;

    void handleHttpRequest(QTcpSocket *clientSocket, const HttpRequest &request);
    void addToCache(const HttpRequest &request);
    void handleConnectionFailure(QTcpSocket *clientSocket, const QString &host, const QString &error);
    bool shouldBlockRequest(const QString &url, const QHash<QString, QString> &headers) ;

};

#endif // HTTPPROXY_H
