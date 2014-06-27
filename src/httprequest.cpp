#include "httprequest.h"

#include <QStringList>

HttpRequest::HttpRequest(QByteArray raw)
{
    mRaw = raw;
    mPath = QString(raw).split(" ").value(1);
}

QString HttpRequest::path() const
{
    return mPath;
}

QByteArray HttpRequest::raw() const
{
    return mRaw;
}
