#include <QCoreApplication>

#include "httpserver.h"

#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    HttpServer http;    // server starts upon creation

    return a.exec();
}
