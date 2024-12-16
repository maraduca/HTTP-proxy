#include "httprequest.h"

HttpRequest::HttpRequest(const QString &method, const QString &url,
                         const QHash<QString, QString> &headers,
                         const QByteArray &body)
    : method(method), url(url), headers(headers), body(body) {}

QString HttpRequest::getMethod() const {
    return method;
}

QString HttpRequest::getUrl() const {
    return url;
}

QHash<QString, QString> HttpRequest::getHeaders() const {
    return headers;
}

QByteArray HttpRequest::getBody() const {
    return body;
}

void HttpRequest::setBody(const QByteArray &newBody) {
    body = newBody;
}

void HttpRequest::debugPrint() const
{
    qDebug() << "HttpRequest Debug:";
    qDebug() << "Method:" << method;
    qDebug() << "URL:" << url;
    qDebug() << "Headers:" << headers;
    qDebug() << "Body size:" << body.size();
}


HttpRequest HttpRequest::parse(const QByteArray &rawRequest)
{
    QString requestLine;
    QHash<QString, QString> headers;
    QByteArray body;

    int headerEndIndex = rawRequest.indexOf("\r\n\r\n");
    if (headerEndIndex != -1)
    {
        QByteArray headerSection = rawRequest.left(headerEndIndex);
        body = rawRequest.mid(headerEndIndex + 4);  // După \r\n\r\n

        headers = parseHeaders(headerSection);
    }
    else
    {
        headers = parseHeaders(rawRequest);
    }

    QList<QByteArray> lines = rawRequest.split('\n');
    if (!lines.isEmpty())
    {
        QList<QByteArray> requestParts = lines[0].split(' ');
        if (requestParts.size() >= 2)
        {
            QString method = QString::fromUtf8(requestParts[0]);
            QString url = QString::fromUtf8(requestParts[1]);
            return HttpRequest(method, url, headers, body);
        }
    }

    return HttpRequest();
}

QHash<QString, QString> HttpRequest::parseHeaders(const QByteArray &rawHeaders)
{
    QHash<QString, QString> parsedHeaders;
    QList<QByteArray> lines = rawHeaders.split('\n');

    //sar peste prima linie
    for (int i = 1; i < lines.size(); ++i)
    {
        QByteArray line = lines[i].trimmed();
        if (line.isEmpty()) continue;

        int separatorIndex = line.indexOf(':');
        if (separatorIndex != -1)
        {
            QString key = QString::fromUtf8(line.left(separatorIndex).trimmed());
            QString value = QString::fromUtf8(line.mid(separatorIndex + 1).trimmed());
            parsedHeaders.insert(key, value);
        }
    }
    return parsedHeaders;
}


QByteArray HttpRequest::headersToRaw() const
{
    QByteArray rawHeaders;
    for (auto it = headers.begin(); it != headers.end(); ++it)
    {
        rawHeaders += it.key().toUtf8() + ": " + it.value().toUtf8() + "\r\n";
    }
    return rawHeaders;
}

QByteArray HttpRequest::toRawRequest() const
{
    return method.toUtf8() + " " + url.toUtf8() + " HTTP/1.1\r\n" +
           headersToRaw() + "\r\n" +
           body;
}
