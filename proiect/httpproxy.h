#ifndef HTTPPROXY_H
#define HTTPPROXY_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class HttpProxy : public QObject
{
    Q_OBJECT

public:
    explicit HttpProxy(int port, QObject *parent = nullptr);
    ~HttpProxy();
    bool start();
    void stop();

signals:
    void logMessage(const QString &msg);

private slots:
    void onNewConnection();
    void handleClient();
    void handleConnect(QTcpSocket *clientSocket, QTcpSocket *serverSocket);

private:
    QTcpServer *server;
    int port;
};

#endif // HTTPPROXY_H
