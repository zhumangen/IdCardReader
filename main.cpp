#include <QCoreApplication>
#include "cardreader.h"
#include "initwlscp.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    InitWLScp scp;
    scp.init();

    CardReader reader;
    reader.connect();
    reader.readLoop();

    return a.exec();
}
