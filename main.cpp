#include <QCoreApplication>
#include "myserver.h"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    MyServer server(8080);
    return a.exec();
}
