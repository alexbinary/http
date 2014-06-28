#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QObject>
#include <QTcpServer>

class ThreadedTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit ThreadedTcpServer(QObject *parent = 0);
    void start();

private:
    void incomingConnection(int socketDescriptor);
    void error(QString);
    void info(QString);

private slots:
    void onAboutToQuit();
};

#endif // HTTPSERVER_H
