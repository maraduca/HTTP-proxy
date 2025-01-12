#include "httpcache.h"

HttpCache::HttpCache() {}

HttpCache::~HttpCache() {}

bool HttpCache::contains(const QString &url) const {
    return cache.contains(url) && cacheTimes.value(url).secsTo(QDateTime::currentDateTime()) < cacheLifetime;
}

QByteArray HttpCache::getResponse(const QString &url) const {
    if (contains(url)) {
        return cache.value(url);
    }
    return QByteArray();  // Returnează un QByteArray gol dacă nu există în cache
}

void HttpCache::addResponse(const QString &url, const QByteArray &response) {
    cache.insert(url, response);
    cacheTimes.insert(url, QDateTime::currentDateTime());
}
