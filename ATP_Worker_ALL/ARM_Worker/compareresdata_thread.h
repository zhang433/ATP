#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif
#pragma once
#include <QObject>
#include <QByteArray>
#include "qqueue.h"
#include "qdatastream.h"
#include "self_define.h"
#include "basestructer.h"
#include "analyze_resmessage.h"
#include "designdata.h"
#include "analyze_atpmessage.h"
#include "Command.h"



enum FilterType {
    None = 0x00,
    Num = 0x01,
    LowerLetter = 0x02,
    UpperLetter = 0x04,
    Chinese = 0x80
};

typedef struct
{
	qint32 D_LINK;
	QString ID;
	qint32 AllowErr;
	Train_Dir Dir;
}Record_Baliseposition;

class CompareBaliseData_THREAD : public QObject
{
    Q_OBJECT
public:
    explicit CompareBaliseData_THREAD(QObject *parent = 0);
    enum Dir m_Direction;
    enum Train_Dir m_TrainDirection;
    enum Line_Dir m_LineDirection;
    QVector<Train_Dir> m_trainDirectionVec; //列车运行方向映射表


private:
    QMap<QString,BalisePosition>& test = DesignData::baliseLocationUpMap;
	QMap<Balish_Category, QString> Balish_Category2Str;

	RealTimeDatastructure RTD;
    QVector<unsigned char> ATPBitData;
	QByteArray QBA;
	BalisePackage Temp_ETCS_5;
	Analyze_ATPMessage *_AM = new Analyze_ATPMessage();

    QList<Record_Baliseposition> ALGI;//AllLinkedGroupID，连接的所有应答器组
	QString NBGI;//该变量的意思为NextBaliseGroupID，下一组链接应答器的编号
	bool SideLineFlag = false;//用于指示是否要进行侧线的逻辑比对
	bool New_BalishGroup = false;
    Balish_Category SideLineIndex = UNKNOWN_Balish_Category;//应答器的用途
	AccessRode_MapType SideLineInfo,SideLineInfoIn, SideLineInfoOut;//要比对的侧线逻辑信息

    int m_udpDataOrder;
    BalisePackCollector m_baliseData_Package;
    BaliseBitMessage m_originBaliseBitMessage;
    QString m_Balise_IDstr;
    QString m_Balise_GroupID,m_LastBalise_GroupID;
    int m_baliseLocation;
	int sideLine_startSigKm;
	int sideLine_endSigKm;

    //QQueue<Record_Baliseposition> LinkedBliseGroupID(BalisePackCollector& BPK); //获取本应答器的下一个连接应答器组的ID
    //QVector<QVector<QString> > Compare_BaliseLocation(int DataType); //比对应答器位置
    QVector<QVector<QString> > Compare_TrackCircuit(int position);//比对轨道电路
    QVector<QVector<QString> > Compare_NeutralSection(int position);//比对分相区 -逻辑验证通过
    QVector<QVector<QString> > Compare_Grade(int position); //比对坡度 -逻辑验证通过

    void judjeGradeCombine(QVector<int> &gradeVec_sheet, QVector<int> &lengthVec_sheet, int& now_index, const int grade_balise, const int len_balise, int &grade_combine, int &len_combine);//用来判断是否存在坡度合并

	void judjeTrackCircuitCombine(QVector<PathWayData_Less> &pathWayData_LessVec, int &now_index, const int& length_balise, const QString& signalType_balise, const QString& fre_balise, 
		int &length_combine, QString& signalType_combine, QString& fre_combine);

    void JudgeSideLineBaliseGroup(const QList<QString>& Balises);
    QString BaliseUseage(QString BaliseGroupNumber);

    QVector<QVector<QString> > Compare_Speed(int position); //比对线路速度 -逻辑验证通过
    QVector<QVector<QString> > Compare_TrackTemporarySpeedLimit(int position); //比对临时限速
    QVector<QString> CompareStringVec(QVector<QString>&str1, QVector<QString>&str2); //判断两个字符串容器对应项是否相等
	QVector<QString> CompareGradeVec(QVector<QString>&str1, QVector<QString>&str2); //判断坡度信息是否相等
	QVector<QString> CompareStringVec_Km(QVector<QString>&str_balise, QVector<QString>&str_sheet); //判断公里标是否相等，并给出误差
	QVector<QString> CompareSpeedVec(QVector<QString>&str_balise, QVector<QString>&str_sheet, bool isAddKm); //判断公里标是否相等，并给出误差
    QVector<QString> CompareSpeedLenVec(QVector<QString>& str_baliseVec, QVector<QString>& str_sheetVec);
    void FillRTD_by_AMData(RealTimeDatastructure &RTD, Analyze_ATPMessage* AM,bool has_compare_result,qint64 timestamp);
    void FillRTD_by_DMSData(RealTimeDatastructure &RTD, QByteArray &datastream, quint8 DMStype);
	int convert2Km(const QString & str);
    QVector<QVector<QVector<QString>>>  resultVec;
    void CompareBaliseData(int DataType);//DataType=0表示应答器报文来自ATP，1表示应答器报文来自DMS

    int BinarySearchTrackCircuit(QVector<PathWayData>& pathWayData, int baliseLocation, int kmAddFlag);
    int JudjeBrokenLink(QVector<BrokenLink> &brokenLink, int x, int y);
    QString strFilter(const QString &input, const QString& filter, FilterType filtertype, bool flag);

    int BinarySearchPathWaySpeed(QVector<PathWaySpeed> &pathWaySpeed, int baliseLocation, int kmAddFlag);
signals:
    void hasCompareResult_udp(QVector<QVector<QString> >, CompareType compareType);
private slots:
    void on_DMSTcp_DataCome(QByteArray datastream,quint8 type);
    void on_UdpThread_DataCome(qint32 order,qint64 timestamp);
};

extern CompareBaliseData_THREAD* CRD_THREAD;
