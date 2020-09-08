#ifndef CARDREADER_H
#define CARDREADER_H

#include <QObject>
#include <windows.h>

#include "cardinfo.h"

class CardReader : public QObject
{
    Q_OBJECT
public:
    explicit CardReader(QObject *parent = nullptr);
    ~CardReader();

    void connect();
    void readLoop();

signals:
    void cardInfoReady(const CardInfo info);

public slots:

private:
    int(__stdcall* InitCommExt)();
    int(__stdcall* CloseComm)();
    int(__stdcall* Authenticate)();
    int(__stdcall* Read_Content)(int Active);
    int(__stdcall* GetBmpPhoto)(const char* pszPath);
    void(__stdcall* getName)(char* szData, int cbData);
    void(__stdcall* getSex)(char* szData, int cbData);
    void(__stdcall* getNation)(char* szData, int cbData);
    void(__stdcall* getBirthdate)(char* szData, int cbData);
    void(__stdcall* getAddress)(char* szData, int cbData);
    void(__stdcall* getIDNum)(char* szData, int cbData);
    void(__stdcall* getIssue)(char* szData, int cbData);
    void(__stdcall* getEffectedDate)(char* szData, int cbData);
    void(__stdcall* getExpiredDate)(char* szData, int cbData);

private:
    HINSTANCE m_hTermbInst;
    bool m_bConnect;
    int m_count;

    void saveAndConvert(const CardInfo &info);
};

#endif // CARDREADER_H
