#include "httpresponse.h"

#include <QTextStream>
#include <QDebug>

#define SERVER_NAME "http-indev"

HttpResponse::HttpResponse()
{
    mDirty = true;
    mHeadDirty = true;
}

HttpResponse HttpResponse::buildResponse(int status, const QString &mimeType, const QByteArray &body)
{
    HttpResponse rep = buildStatusResponse(status);

    rep.setMimeType(mimeType);
    rep.setBody(body);

    return rep;
}

HttpResponse HttpResponse::buildResponse(int status, const QString &mimeType, QFile& file)
{
    HttpResponse rep = buildStatusResponse(status);

    rep.setMimeType(mimeType);
    rep.setBody(file);

    return rep;
}

HttpResponse HttpResponse::buildResponse(int status, const QString &mimeType, const QString& filePath)
{
    HttpResponse rep = buildStatusResponse(status);

    rep.setMimeType(mimeType);
    rep.setBody(filePath);

    return rep;
}

HttpResponse HttpResponse::buildStatusResponse(int status)
{
    HttpResponse rep;

    rep.setStatus(status);

    return rep;
}

QByteArray HttpResponse::raw() const
{
    if(mDirty) {

        mRaw.clear();
        QTextStream responseStream(&mRaw);

        responseStream << rawHead();

        responseStream << mBody;
        responseStream << "\r\n";

        responseStream.flush();

        mDirty = false;
    }

    return mRaw;
}

QByteArray HttpResponse::rawHead() const
{
    if(mHeadDirty) {

        /*
         * Content-Length header is required for the client to know
         * when all the data has been received
         *
         * Content-Type might be needed for the client to process
         * the data correctly
         *
         * User-defined headers take over calculated headers
         */

        QMap<QString,QString> headers = mHeaders;

        if(!headers.contains("Content-Type")) {
            headers["Content-Type"] = mMimeType;
        }
        if(!headers.contains("Content-Length")) {
            headers["Content-Length"] = QString::number(mBody.size());
        }
        if(!headers.contains("Content-Length")) {
            headers["Content-Length"] = QString::number(mBody.size());
        }
        if(!headers.contains("Server")) {
            headers["Server"] = SERVER_NAME;
        }

        mRawHead.clear();
        QTextStream responseStream(&mRawHead);

        responseStream << "HTTP/1.1 " << mStatus;
        responseStream << "\r\n";

        foreach(const QString& key,headers.keys()) {
            responseStream << key << ": " << headers[key];
            responseStream << "\r\n";
        }

        responseStream << "\r\n";
        responseStream.flush();

        mHeadDirty = false;
    }

    return mRawHead;
}

void HttpResponse::setBody(const QByteArray &body)
{
    mBody = body;
    mDirty = true;
}

void HttpResponse::setBody(QFile &file)
{
    file.open(QIODevice::ReadOnly);
    setBody(file.readAll());
    file.close();
}

void HttpResponse::setBody(const QString &filePath)
{
    QFile file(filePath);
    setBody(file);
}

void HttpResponse::setHeader(const QString &header, const QString &value)
{
    mHeaders[header] = value;
    mDirty = true;
    mHeadDirty = true;
}

void HttpResponse::setMimeType(const QString &mimeType)
{
    mMimeType = mimeType;
    mDirty = true;
    mHeadDirty = true;
}

void HttpResponse::setStatus(int status)
{
    mStatus = status;
    mDirty = true;
    mHeadDirty = true;
}
