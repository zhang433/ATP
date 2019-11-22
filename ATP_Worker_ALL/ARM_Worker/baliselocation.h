#ifndef BALISELOCATION_H
#define BALISELOCATION_H
#include <QQueue>
#include "compareresdata_thread.h"

enum class LineType{
    Normal = 0,
    SideLineIn = 1,
    SideLine = 2,
    SideLineOut = 3
};

class BaliseLocation
{
public:
    BaliseLocation();
    static QList<Record_Baliseposition> LinkedBliseGroupID(const BalisePackCollector& BPK);
    static QString BaliseUseage(const QString BaliseGroupNumber);
    static QList<LineType> JudgeSideLineBaliseGroup(const QList<QString>& Balises,const int index);
    static QVector<QVector<QString>> Compare(const BalisePackCollector& BPK,QList<QString> RecordBaliseGroups);
    static AccessRode_MapType SideLineInfoIn,SideLineInfoOut;
private:
    static AccessRode_MapType GetSideLineInfo(const QString BaliseGroup,const QString nextBaliseGroup);//找到应答器的侧线信息
    static int FindSideLineOutNearby(const QList<LineType>& labels,int index);//找到距离index位置最近的SidelineIn标记
    static int FindSideLineInNearby(const QList<LineType>& labels,int index);//找到距离index位置最近的SideLindeOut标记
};

#endif // BALISELOCATION_H
