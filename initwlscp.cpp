#include "initwlscp.h"
#include <QDir>
#include <QFile>
#include <QProcess>

InitWLScp::InitWLScp()
{

}

void InitWLScp::init()
{
    QString root = "C:/IDCardDb";
    QString db = root + "/data";
    QString ae = db + "/IDCARD";
    QString temp = root + "/temp";
    QDir dir;
    dir.mkpath(ae);
    dir.mkpath(temp);

    QFile lock(ae + "/lockfile");
    lock.open(QIODevice::ReadWrite);
    lock.close();

    wlmscp.setProgram("wlmscpfs.exe");
    wlmscp.setNativeArguments("-dfp " + db);
    wlmscp.setArguments(QStringList() << "3000");
    wlmscp.start();
}
