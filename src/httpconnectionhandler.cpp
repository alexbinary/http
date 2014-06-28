#include "httpconnectionhandler.h"

#include <QFileInfo>
#include <QFile>
#include <QStringList>
#include <QHostAddress>

#define SERVER_ROOT "../www"
#define SERVER_STATUS_ROOT "/status"

HttpConnectionHandler::HttpConnectionHandler(int socketDescriptor, QObject *parent) :
    QObject(parent),mSocketDescriptor(socketDescriptor)
{
}

HttpResponse HttpConnectionHandler::buildFileResponse(const QString& filePath)
{
    return HttpResponse::buildResponse(
                200,
                guessMimeTypeFromFileName(filePath),
                filePath
                );
}

HttpResponse HttpConnectionHandler::buildStatusResponse(int status)
{
    HttpResponse rep = HttpResponse::buildStatusResponse(status);
    rep.setMimeType("text/html");

    QString path = SERVER_ROOT;
    path += SERVER_STATUS_ROOT;
    path += "/" + QString::number(status) + ".html";

    QFileInfo fi(path);
    if(fi.exists()) {
        rep.setBody(path);
    } else {
        rep.setBody(QString::number(status));
    }

    return rep;
}

void HttpConnectionHandler::error(QString e)
{
    QTextStream stderrStream(stderr);
    stderrStream << mConnectionId << ": Error: " << e << endl << endl;
}

QString HttpConnectionHandler::guessMimeTypeFromFileName(const QString &fileName)
{
    QString mimeType;
    QMap<QString,QString> mimeTypeMap;

    mimeTypeMap["html"] = "text/html";
    mimeTypeMap["txt"]  = "text/plain";
    mimeTypeMap["css"]  = "text/css";
    mimeTypeMap["jpeg"] = "image/jpeg";
    mimeTypeMap["jpg"]  = "image/jpeg";
    mimeTypeMap["png"]  = "image/png";

    QFileInfo fi(fileName);
    return mimeTypeMap.value(fi.suffix().toLower(),"application/octet-stream");
}

void HttpConnectionHandler::info(QString s)
{
    QTextStream stdoutStream(stdout);
    stdoutStream << mConnectionId << ": " << s << endl << endl;
}

void HttpConnectionHandler::processRequest(HttpRequest r)
{
    sendResponse(responseForRequest(r));
}

HttpResponse HttpConnectionHandler::responseForRequest(const HttpRequest & r)
{
    QString path = r.path();

    // 1. filter *dangerous* requests

    if(path.contains("/.."))
    {
        return buildStatusResponse(403);
    }

    // 2. translate requests

    if(path=="/")
    {
        path = "/index.html";
    }

    // 3. check if requested file exists

    QFileInfo fi(SERVER_ROOT + path);
    if(!fi.exists())
    {
        return buildStatusResponse(404);
    }

    // 4. serve requested file

    return buildFileResponse(fi.filePath());
}

void HttpConnectionHandler::sendResponse(const HttpResponse& response)
{
    info("Response");

    QByteArray data = response.raw();
    info(response.rawHead());

    mTcpSocket->write(data);
    mTcpSocket->flush();
}

void HttpConnectionHandler::sendStatusResponse(int status)
{
    sendResponse(buildStatusResponse(status));
}

void HttpConnectionHandler::serveFile(const QString &filePath)
{
    sendResponse(buildFileResponse(filePath));
}

void HttpConnectionHandler::socketDisconnected()
{
    info("Client disconnected");
    emit connectionClosed();
}

void HttpConnectionHandler::socketError(QAbstractSocket::SocketError e)
{
    error(mTcpSocket->errorString());
}

void HttpConnectionHandler::socketReadyRead()
{
    info("Request");

    QByteArray data = mTcpSocket->readAll();

    HttpRequest req(data);
    info(req.raw());

    processRequest(req);
}

void HttpConnectionHandler::start()
{
    mConnectionId = qrand();

    mTcpSocket = new QTcpSocket(this);
    if(!mTcpSocket->setSocketDescriptor(mSocketDescriptor)) {
        error(mTcpSocket->errorString());
        return;
    }

    info("Client connected from "+mTcpSocket->peerAddress().toString()+":"+QString::number(mTcpSocket->peerPort()));

    connect(mTcpSocket,SIGNAL(readyRead()),
            this,SLOT(socketReadyRead()));
    connect(mTcpSocket,SIGNAL(disconnected()),
            this,SLOT(socketDisconnected()));
    connect(mTcpSocket,SIGNAL(error(QAbstractSocket::SocketError)),
            this,SLOT(socketError(QAbstractSocket::SocketError)));
}


