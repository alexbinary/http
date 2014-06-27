#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <QString>
#include <QByteArray>

class HttpRequest
{
public:
    HttpRequest(QByteArray raw);
    QByteArray raw() const;
    QString path() const;

private:
    QByteArray mRaw;
    QString mPath;
};

#endif // HTTPREQUEST_H
