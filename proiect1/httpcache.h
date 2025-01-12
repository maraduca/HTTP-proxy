#ifndef HTTPCACHE_H
#define HTTPCACHE_H


#include <QHash>
#include <QByteArray>
#include <QString>
#include <QDateTime>

class HttpCache
{
public:
    HttpCache();
    ~HttpCache();

    bool contains(const QString &url) const;
    QByteArray getResponse(const QString &url) const;
    void addResponse(const QString &url, const QByteArray &response);

private:
    QHash<QString, QByteArray> cache;  // Cheia este URL-ul, valoarea este răspunsul
    QHash<QString, QDateTime> cacheTimes;  // Timpul la care a fost salvat răspunsul
    const int cacheLifetime = 3600;  // Durata de viață a unui răspuns în cache (în secunde)
};

#endif // HTTPCACHE_H
