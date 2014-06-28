#include <QCoreApplication>

#include "threadedtcpserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    ThreadedTcpServer server;
    server.start();

    return a.exec();
}
