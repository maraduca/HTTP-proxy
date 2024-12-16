#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <QByteArray>
#include <QString>
#include <QList>
#include <QHash>
#include <QDebug>

class HttpRequest {
    QString method;
    QString url;
    QHash<QString, QString> headers;
    QByteArray body; // Cererea inițială sau răspunsul serverului

public:

    HttpRequest(const QString &method = "", const QString &url = "",
                const QHash<QString, QString> &headers = {}, const QByteArray &body = {});

    QString getMethod() const;
    QString getUrl() const;
    QHash<QString, QString> getHeaders() const;
    QByteArray getBody() const;


    void setBody(const QByteArray &newBody);


    static HttpRequest parse(const QByteArray &rawRequest);
    static QHash<QString, QString> parseHeaders(const QByteArray &rawHeaders);

    // Funcții pentru serializare
    QByteArray headersToRaw() const;
    QByteArray toRawRequest() const;
    void debugPrint()const ;
};


#endif // HTTPREQUEST_H
