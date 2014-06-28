#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include <QString>
#include <QMap>
#include <QFile>

class HttpResponse
{
public:
    HttpResponse();

    QByteArray raw() const;
    QByteArray rawHead() const;
    void setStatus(int);
    void setMimeType(const QString&);
    void setHeader(const QString& header,const QString& value);
    void setBody(const QByteArray&);
    void setBody(QFile &file);
    void setBody(const QString& filePath);

    static HttpResponse buildResponse(int status, const QString &mimeType, const QByteArray &body);
    static HttpResponse buildResponse(int status, const QString &mimeType, QFile& file);
    static HttpResponse buildResponse(int status, const QString &mimeType, const QString& filePath);
    static HttpResponse buildStatusResponse(int status);

private:
    mutable QByteArray mRaw;
    mutable QByteArray mRawHead;
    mutable bool mDirty;
    mutable bool mHeadDirty;

    QString mMimeType;
    int mStatus;
    QMap<QString,QString> mHeaders;

    QByteArray mBody;
};

#endif // HTTPRESPONSE_H
