#ifndef CARDINFO_H
#define CARDINFO_H

#include <QString>
#include <QDate>

class CardInfo
{
public:
    QString id;
    QString name;
    QString sex;
    QDate birth;
    QString age;
    QString address;
};

#endif // CARDINFO_H
