#pragma once
/**********************************************
 * @功能：此类主要存储导入的列控数据
 * @作者：hb
 * @修改日期：2017-5-21
**********************************************/

#include "basestructer.h"
#include <QString>
#include <QVector>
#include <QObject>
#include <QMap>
#include <QStringList>
#include <QMutex>
#include <unordered_map>

class DesignData : public QObject
{
public:
    static QMutex SheetMutex;
    static QMultiMap<QString, AccessRode_MapType> accessRodeMap;//将各车站的进路数据表统一汇总成哈希的形式，key为应答器号，value为要保留的数据信息

    static QVector<Station> stationVec;                   //车站表

    static QVector<Grade> gradeDownProVec;                //坡度表-下行正向
    static QVector<Grade> gradeUpProVec;                  //坡度表-上行正向
    static QVector<Grade> gradeDownBackVec;               //坡度表-下行反向
    static QVector<Grade> gradeUpBackVec;                 //坡度表-上行反向

    static QVector<NeutralSection> neutralSectionUpVec;   //分相区表-上行线
    static QVector<NeutralSection> neutralSectionDownVec; //分相区表-下行线

    static QVector<PathWayData> pathWayDataDownProVec;    //线路数据表-下行正向
    static QVector<PathWayData> pathWayDataUpProVec;      //线路数据表-上行正向
    static QVector<PathWayData> pathWayDataDownBackVec;   //线路数据表-下行反向
    static QVector<PathWayData> pathWayDataUpBackVec;     //线路数据表-上行反向

    static QVector<PathWaySpeed> pathWaySpeedDownVec;     //线路速度表-下行
    static QVector<PathWaySpeed> pathWaySpeedUpVec;       //线路速度表-上行
    static QVector<PathWaySpeed> pathWaySpeedDownBackVec; //线路速度表-下行反向
    static QVector<PathWaySpeed> pathWaySpeedUpBackVec;   //线路速度表-上行反向

    static QMap<QString,BalisePosition> baliseLocationUpMap; //应答器位置表-上行
    static QMap<QString, BalisePosition> baliseLocationDownMap; //应答器位置表-下行
    static QMap<QString, QString> balishUseMap;

    static QVector<BrokenLink> brokenLinkVec;             //里程断链明细表

    static QVector<Coordinate> coordinateVec;             //坐标系信息表

    static QVector<KmInfo> kmInfoVec;                     //里程信息表

    static QVector<StationSide> stationSideVec;           //站台侧信息表

    static void clearAll();

    static int ConvertKmStr2Num(QString input);
    static int ConvertLinkMark2Num(const QString linStr);

    static void clearAccessRodeVec();
    static void clearStationVec();
    static void clearGradeVec();
    static void clearNeutralSectionVec();
    static void clearPathWayDataVec();
    static void clearPathWaySpeedVec();
    static void clearBaliseLocationVec();
    static void clearBrokenLinkVec();
    static void clearStationSideVec();

    static int FindBaliseKmByID(QString baliseID); //change
	static QString FindBaliseRemarkByID(QString baliseID); //change
};

/**************************************************************
 *功能：将坡度表和线路速度表中备注栏的起点位置转化为结构体T加入到容器首或尾
 *形参：QVector<T> &vec -存放表格的原始容器
 *返回值：QVector<T> 处理之后的表格容器
 *注意：此函数只能用来处理坡度表和速度表
 *************************************************************/
template <typename T>
QVector<T>  processVector(QVector<T> vec)
{
    T tempT;
    int posT = -1;
    QString tempStr = "";
    QStringList strList;
    for(int i = 0; i < vec.size(); i++)
    {
       if(vec[i].remark.contains("起点"))
       {
           strList = vec[i].remark.split(","); //因为备注栏中的起点可能会和断链在一起描述，所以要截取出起点描述
           if(strList.size() == 1)
           {
               strList = vec[i].remark.split("，");
           }
           for(int k = 0; k < strList[0].size(); k++)
           {
               if(strList[0][k] >= '0' && strList[0][k] <= '9')
               {
                   tempStr.append(strList[0][k]);
               }
           }
           tempT.endKm = tempStr;
           posT = i;
           break;
       }
    }
    if(posT == 0)
    {
        vec.insert(vec.begin(), tempT);
    }
    if(posT == vec.size() - 1)
    {
        vec.append(tempT);
    }
    return vec;
}
