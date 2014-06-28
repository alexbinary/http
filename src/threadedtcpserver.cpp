#include "threadedtcpserver.h"

#include <QCoreApplication>
#include <QThread>

#include "httpconnectionhandler.h"

#define SERVER_PORT 8080

ThreadedTcpServer::ThreadedTcpServer(QObject *parent) :
    QTcpServer(parent)
{
    connect(qApp,SIGNAL(aboutToQuit()),
            this,SLOT(onAboutToQuit()));
}

void ThreadedTcpServer::error(QString e)
{
    QTextStream stderrStream(stderr);
    stderrStream << "Server: Error: " << e << endl << endl;
}

void ThreadedTcpServer::incomingConnection(int socketDescriptor)
{
    HttpConnectionHandler* connectionHandler = new HttpConnectionHandler(socketDescriptor);
    // don't set parent to connectionHandler because it will be moved to another thread
    QThread* connectionThread = new QThread(this);

    connectionHandler->moveToThread(connectionThread);

    connect(connectionThread,SIGNAL(started()),
            connectionHandler,SLOT(start()));
    connect(connectionHandler,SIGNAL(connectionClosed()),
            connectionHandler,SLOT(deleteLater()));
    connect(connectionHandler,SIGNAL(connectionClosed()),
            connectionThread,SLOT(quit()));
    connect(connectionThread,SIGNAL(terminated()),
            connectionThread,SLOT(deleteLater()));

    connectionThread->start();
}

void ThreadedTcpServer::info(QString s)
{
    QTextStream stdoutStream(stdout);
    stdoutStream << "Server: " << s << endl << endl;
}

void ThreadedTcpServer::onAboutToQuit()
{
    info("quitting");
}

void ThreadedTcpServer::start()
{
    if(listen(QHostAddress::Any,SERVER_PORT)) {
        info("Server is listening on port "+QString::number(serverPort()));
    } else {
        error(errorString());
    }
}
