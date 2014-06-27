#include "httpserver.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QFile>
#include <QStringList>

#define SERVER_PORT 8080
#define SERVER_ROOT "../www"
#define SERVER_STATUS_ROOT "/status"

HttpServer::HttpServer(QObject *parent) :
    QObject(parent)
{
    info("Starting...");

    connect(qApp,SIGNAL(aboutToQuit()),
            this,SLOT(onAboutToQuit()));

    mTcpServer = new QTcpServer(this);

    connect(mTcpServer,SIGNAL(newConnection()),
            this,SLOT(newConnection()));
    connect(mTcpServer,SIGNAL(acceptError(QAbstractSocket::SocketError)),
            this,SLOT(acceptError(QAbstractSocket::SocketError)));

    if(mTcpServer->listen(QHostAddress::Any,SERVER_PORT))
    {
        info("Server is listening on port "+QString::number(mTcpServer->serverPort()));
    }
    else
    {
        error(mTcpServer->errorString());
    }
}

void HttpServer::acceptError(QAbstractSocket::SocketError e)
{
    error(mTcpServer->errorString());
}

HttpResponse HttpServer::buildFileResponse(const QString& filePath)
{
    return HttpResponse::buildResponse(
                200,
                guessMimeTypeFromFileName(filePath),
                filePath
                );

}

HttpResponse HttpServer::buildStatusResponse(int status)
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

void HttpServer::error(QString e)
{
    QTextStream mStderrStream(stderr);
    mStderrStream << "Error: " << e << endl << endl;
}

QString HttpServer::guessMimeTypeFromFileName(const QString &fileName)
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

void HttpServer::info(QString s)
{
    QTextStream mStdoutStream(stdout);
    mStdoutStream << s << endl << endl;
}

void HttpServer::newConnection()
{
    mTcpSocket = mTcpServer->nextPendingConnection();
    if(mTcpSocket==0)
        return;

    connect(mTcpSocket,SIGNAL(readyRead()),
            this,SLOT(socketReadyRead()));
    connect(mTcpSocket,SIGNAL(disconnected()),
            this,SLOT(socketDisconnected()));
    connect(mTcpSocket,SIGNAL(error(QAbstractSocket::SocketError)),
            this,SLOT(socketError(QAbstractSocket::SocketError)));

    info("Client connected from "+mTcpSocket->peerAddress().toString()+":"+QString::number(mTcpSocket->peerPort()));
}

void HttpServer::onAboutToQuit()
{
    info("quitting");
}

void HttpServer::processRequest(HttpRequest r)
{
    sendResponse(responseForRequest(r));
}

HttpResponse HttpServer::responseForRequest(const HttpRequest & r)
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

void HttpServer::sendResponse(const HttpResponse& response)
{
    QByteArray data = response.raw();

    mTcpSocket->write(data);
    mTcpSocket->flush();
}

void HttpServer::sendStatusResponse(int status)
{
    sendResponse(buildStatusResponse(status));
}

void HttpServer::serveFile(const QString &filePath)
{
    sendResponse(buildFileResponse(filePath));
}

void HttpServer::socketDisconnected()
{
    info("Client disconnected");
}

void HttpServer::socketError(QAbstractSocket::SocketError e)
{
    error(mTcpSocket->errorString());
}

void HttpServer::socketReadyRead()
{
    QByteArray data = mTcpSocket->readAll();

    HttpRequest req(data);
    info(req.raw());

    processRequest(req);
}
