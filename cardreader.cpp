#include "CardReader.h"

#include <QTextStream>
#include <QFile>
#include <QProcess>
#include <QUuid>

#define BUFFER_SIZE 256

CardReader::CardReader(QObject *parent) :m_bConnect(false),m_hTermbInst(NULL),m_count(0),QObject(parent)
{

    m_hTermbInst = LoadLibrary(QString::fromLocal8Bit("termb.dll").toStdWString().data());
    if (m_hTermbInst)
    {
        InitCommExt = (int(__stdcall*)()) GetProcAddress(m_hTermbInst, "InitCommExt");
        CloseComm = (int(__stdcall*)()) GetProcAddress(m_hTermbInst, "CloseComm");
        Authenticate = (int(__stdcall*)()) GetProcAddress(m_hTermbInst, "Authenticate");
        Read_Content = (int(__stdcall*)(int)) GetProcAddress(m_hTermbInst, "Read_Content");
        GetBmpPhoto = (int(__stdcall*)(const char*)) GetProcAddress(m_hTermbInst, "GetBmpPhoto");
        getName = (void(__stdcall*)(char*, int)) GetProcAddress(m_hTermbInst, "getName");
        getSex = (void(__stdcall*)(char*, int)) GetProcAddress(m_hTermbInst, "getSex");
        getNation = (void(__stdcall*)(char*, int)) GetProcAddress(m_hTermbInst, "getNation");
        getBirthdate = (void(__stdcall*)(char*, int)) GetProcAddress(m_hTermbInst, "getBirthdate");
        getAddress = (void(__stdcall*)(char*, int)) GetProcAddress(m_hTermbInst, "getAddress");
        getIDNum = (void(__stdcall*)(char*, int)) GetProcAddress(m_hTermbInst, "getIDNum");
        getIssue = (void(__stdcall*)(char*, int)) GetProcAddress(m_hTermbInst, "getIssue");
        getEffectedDate = (void(__stdcall*)(char*, int)) GetProcAddress(m_hTermbInst, "getEffectedDate");
        getExpiredDate = (void(__stdcall*)(char*, int)) GetProcAddress(m_hTermbInst, "getExpiredDate");

        if (!InitCommExt || !CloseComm || !Authenticate || !Read_Content || !GetBmpPhoto || !getName)
        {
            FreeLibrary(m_hTermbInst);
            m_hTermbInst = NULL;
        }
    }
    else
    {
         qFatal("load termb.dll failed.");
    }
}

CardReader::~CardReader() {
    if (!m_hTermbInst)
    {
        FreeLibrary(m_hTermbInst);
        m_hTermbInst = NULL;
    }
    if (m_bConnect)
    {
        CloseComm();
    }
}

void CardReader::connect() {
    if (!m_hTermbInst || m_bConnect)
    {
        return;
    }
    if (0 >= InitCommExt())
    {
        qFatal("connect failed.");
        return;
    }

    m_bConnect = true;
}

void CardReader::readLoop()
{
    if (!m_bConnect) {
        return;
    }

    while (1) {
        char szData[BUFFER_SIZE] = { 0x0 };

        int nRet = 0;
        nRet = Authenticate();
        if (nRet == 1) {
            nRet = Read_Content(1);
            if (nRet == 1 || nRet == -101) {
                // 读卡成功
                CardInfo info;

                memset(szData, 0x0, BUFFER_SIZE);
                getIDNum(szData, BUFFER_SIZE);
                info.id = QString::fromLocal8Bit(szData);

                memset(szData, 0x0, BUFFER_SIZE);
                getName(szData, BUFFER_SIZE);
                info.name = QString::fromLocal8Bit(szData);

                memset(szData, 0x0, BUFFER_SIZE);
                getSex(szData, BUFFER_SIZE);
                QString sex = QString::fromLocal8Bit(szData);
                if (sex == "男") {
                    info.sex = 'M';
                } else if (sex == "女") {
                    info.sex = 'F';
                } else {
                    info.sex = 'O';
                }

                memset(szData, 0x0, BUFFER_SIZE);
                getBirthdate(szData, BUFFER_SIZE);
                info.birth = QDate::fromString(QString::fromLatin1(szData), "yyyyMMdd");

                info.age = QString::number(QDate::currentDate().year() - info.birth.year()) + "Y";

                memset(szData, 0x0, BUFFER_SIZE);
                getAddress(szData, BUFFER_SIZE);
                info.address = QString::fromLocal8Bit(szData);

                saveAndConvert(info);

            }
        }

        Sleep(300);
    }
}

void CardReader::saveAndConvert(const CardInfo &info)
{
    m_count++;
    QString fileName = QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz") + QString("-%1").arg(m_count, 4, 10, QChar::fromLatin1('0'));
    QFile dump("C:/IDCardDb/temp/" + fileName + ".dump");
    if (dump.open(QIODevice::WriteOnly)) {
        QTextStream out(&dump);

        out << "(0008,0050) SH  " << fileName << endl;
        out << "(0008,0005) CS  [GB18030]"<< endl;
        out << "(0010,0020) LO  " << info.id + "-" + QUuid::createUuid().toString().split("-").at(1) << endl;
        out << "(0010,0010) PN  " << info.name << endl;
        out << "(0010,0040) CS  " << info.sex << endl;
        out << "(0010,0030) DA  " << info.birth.toString("yyyyMMdd") << endl;
        out << "(0010,1040) LO  " << info.address << endl;
        out << "(0010,1010)	AS  " << info.age << endl;
        out << "(0020,000d) UI  1.2.276.0.7230010.3.2.101." + QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz") + QString(".%1").arg(m_count, 4, 10, QChar::fromLatin1('0')) << endl;
        out << "(0032,1032) PN  SMITH" << endl;
        out << "(0032,1060) LO  EXAM6" << endl;
        out << "(0040,0100) SQ" << endl;
        out << "(fffe,e000) -" << endl;
        out << "(0008,0060) CS  DX" << endl;
        out << "(0040,0001) AE  DROC" << endl;
        out << "(0040,0002) DA  " << QDate::currentDate().toString("yyyyMMdd") << endl;
        out << "(0040,0003) TM  " << QTime::currentTime().toString("hhmmss") << endl;
        out << "(0040,0006) PN  JOHNSON" << endl;
        out << "(0040,0007) LO  EXAM74" << endl;
        out << "(0040,0009) SH  SPD3445" << endl;
        out << "(0040,0010) SH  STN456" << endl;
        out << "(0040,0011) SH  B34F56" << endl;
        out << "(0040,0012) LO" << endl;
        out << "(0040,0400) LT" << endl;
        out << "(fffe,e00d) -" << endl;
        out << "(fffe,e0dd) -" << endl;
        out << "(0040,1001) SH  RP454G234" << endl;
        out << "(0040,1003) SH  LOW" << endl;
        out << flush;

        dump.close();

        QStringList args;
        args << dump.fileName();
        args << "C:/IDCardDb/data/IDCARD/" + fileName + ".wl";
        QProcess::startDetached("dump2dcm.exe", args);
    }
}
