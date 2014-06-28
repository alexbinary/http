#ifndef HTTPCONNECTIONHANDLER_H
#define HTTPCONNECTIONHANDLER_H

#include <QObject>
#include <QTcpSocket>

#include "httprequest.h"
#include "httpresponse.h"

class HttpConnectionHandler : public QObject
{
    Q_OBJECT
public:
    explicit HttpConnectionHandler(int socketDescriptor,QObject *parent = 0);

public slots:
    void start();

signals:
    void connectionClosed();

private:
    HttpResponse responseForRequest(const HttpRequest&);
    HttpResponse buildStatusResponse(int status);
    HttpResponse buildFileResponse(const QString& filePath);
    QString guessMimeTypeFromFileName(const QString&);
    void processRequest(HttpRequest req);
    void sendResponse(const HttpResponse&);
    void sendStatusResponse(int status);
    void serveFile(const QString& filePath);
    void error(QString);
    void info(QString);

private slots:
    void socketDisconnected();
    void socketReadyRead();
    void socketError(QAbstractSocket::SocketError);

private:
    int mConnectionId;
    int mSocketDescriptor;
    QTcpSocket* mTcpSocket;
};

#endif // HTTPCONNECTIONHANDLER_H
