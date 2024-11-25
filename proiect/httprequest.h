#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <QByteArray>
#include <QString>
#include <QList>
#include <QHash>

class HttpRequest {
    QString method;
    QString url;
    QHash<QString, QString> headers;
    QByteArray body; // Cererea inițială sau răspunsul serverului

public:
    // Constructori
    HttpRequest(const QString &method = "", const QString &url = "",
                const QHash<QString, QString> &headers = {}, const QByteArray &body = {});

    // Getteri
    QString getMethod() const;
    QString getUrl() const;
    QHash<QString, QString> getHeaders() const;
    QByteArray getBody() const;

    // Setteri
    void setBody(const QByteArray &newBody);

    // Funcții statice pentru parsare
    static HttpRequest parse(const QByteArray &rawRequest);
    static QHash<QString, QString> parseHeaders(const QByteArray &rawHeaders);

    // Funcții pentru serializare
    QByteArray headersToRaw() const;
    QByteArray toRawRequest() const;
};


#endif // HTTPREQUEST_H
