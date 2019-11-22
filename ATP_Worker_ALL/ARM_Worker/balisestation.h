#ifndef BALISESTATION_H
#define BALISESTATION_H
#include <QObject>
#include "self_define.h"

class BaliseStation
{
public:
    BaliseStation();
    static QVector<QVector<QString>> Compare(const BalisePackCollector& BPK,QString& BaliseID);//比对车站名，站台侧信息
    static QString GetStationNameOfBliaseGroup(const QString& Balise);
};

#endif // BALISESTATION_H
