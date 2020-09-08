#ifndef INITWLSCP_H
#define INITWLSCP_H

#include <QProcess>

class InitWLScp
{
public:
    InitWLScp();

    void init();

private:
    QProcess wlmscp;
};

#endif // INITWLSCP_H
