#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

#include "httprequest.h"
#include "httpresponse.h"

class HttpServer : public QObject
{
    Q_OBJECT
public:
    explicit HttpServer(QObject *parent = 0);

public slots:
    void newConnection();
    void acceptError(QAbstractSocket::SocketError);

private:
    void error(QString);
    void info(QString);

    QString guessMimeTypeFromFileName(const QString&);

    void processRequest(HttpRequest req);
    HttpResponse responseForRequest(const HttpRequest&);

    HttpResponse buildStatusResponse(int status);
    HttpResponse buildFileResponse(const QString& filePath);
    void sendResponse(const HttpResponse&);
    void sendStatusResponse(int status);

    void serveFile(const QString& filePath);

private slots:
    void socketDisconnected();
    void socketReadyRead();
    void socketError(QAbstractSocket::SocketError);
    void onAboutToQuit();

private:
    QTcpServer* mTcpServer;
    QTcpSocket* mTcpSocket;
};

#endif // HTTPSERVER_H
