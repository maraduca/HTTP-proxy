#include "httprequest.h"

// Constructor implicit și explicit pentru HttpRequest
HttpRequest::HttpRequest(const QString &method, const QString &url,
                         const QHash<QString, QString> &headers,
                         const QByteArray &body)
    : method(method), url(url), headers(headers), body(body) {}

// Getter pentru metoda HTTP
QString HttpRequest::getMethod() const {
    return method;
}

// Getter pentru URL
QString HttpRequest::getUrl() const {
    return url;
}

// Getter pentru antete
QHash<QString, QString> HttpRequest::getHeaders() const {
    return headers;
}

// Getter pentru corpul cererii sau răspunsului
QByteArray HttpRequest::getBody() const {
    return body;
}

// Setter pentru corpul cererii sau răspunsului
void HttpRequest::setBody(const QByteArray &newBody) {
    body = newBody;
}

// Funcție statică pentru parsarea unei cereri brute
HttpRequest HttpRequest::parse(const QByteArray &rawRequest) {
    QString requestLine;
    QHash<QString, QString> headers;
    QByteArray body;

    int headerEndIndex = rawRequest.indexOf("\r\n\r\n");
    if (headerEndIndex != -1) {
        QByteArray headerSection = rawRequest.left(headerEndIndex);
        body = rawRequest.mid(headerEndIndex + 4);  // După \r\n\r\n

        // Parsăm antetele
        headers = parseHeaders(headerSection);
    } else {
        headers = parseHeaders(rawRequest);
    }

    // Parsăm prima linie pentru metodă și URL
    QList<QByteArray> lines = rawRequest.split('\n');
    if (!lines.isEmpty()) {
        QList<QByteArray> requestParts = lines[0].split(' ');
        if (requestParts.size() >= 2) {
            QString method = QString::fromUtf8(requestParts[0]);
            QString url = QString::fromUtf8(requestParts[1]);
            return HttpRequest(method, url, headers, body);
        }
    }

    return HttpRequest(); // Cerere invalidă
}

// Funcție statică pentru parsarea antetelor brute
QHash<QString, QString> HttpRequest::parseHeaders(const QByteArray &rawHeaders) {
    QHash<QString, QString> parsedHeaders;
    QList<QByteArray> lines = rawHeaders.split('\n');

    // Sărim peste prima linie care conține metoda și URL-ul
    for (int i = 1; i < lines.size(); ++i) {
        QByteArray line = lines[i].trimmed();
        if (line.isEmpty()) continue;

        // Separăm antetul în cheia și valoarea sa
        int separatorIndex = line.indexOf(':');
        if (separatorIndex != -1) {
            QString key = QString::fromUtf8(line.left(separatorIndex).trimmed());
            QString value = QString::fromUtf8(line.mid(separatorIndex + 1).trimmed());
            parsedHeaders.insert(key, value);
        }
    }
    return parsedHeaders;
}

// Funcție pentru a converti antetele într-un format brut
QByteArray HttpRequest::headersToRaw() const {
    QByteArray rawHeaders;
    for (auto it = headers.begin(); it != headers.end(); ++it) {
        rawHeaders += it.key().toUtf8() + ": " + it.value().toUtf8() + "\r\n";
    }
    return rawHeaders;
}

// Funcție pentru a converti cererea într-un format brut
QByteArray HttpRequest::toRawRequest() const {
    return method.toUtf8() + " " + url.toUtf8() + " HTTP/1.1\r\n" +
           headersToRaw() + "\r\n" +
           body;
}
