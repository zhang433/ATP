#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif
#pragma once
/*************************************************************
 * @功能：此头文件中定义了一些基础数据结构体，用在导入列控数据
 * @作者：hb
 * @修改日期：2017-5-21
*************************************************************/



#include <QString>
#include <QVector>
#include <qdatastream.h>

//定义tab页各项位置
#define TAB_POSITION_ZHUJIEMIAN 0
#define TAB_POSITION_YINGDAQIWEIZHI 1
#define TAB_POSITION_GUIDAODIANLU 2
#define TAB_POSITION_XIANLUSUDU 3
#define TAB_POSITION_XIANLUPODU 4
#define TAB_POSITION_CHEZHAN 5
#define TAB_POSITION_FENXINGQU 6
#define TAB_POSITION_LINSHIXIANSU 7


//比较类型
enum CompareType
{
    C_YINGDAQIWEIZHI = 0,
	C_XIANLUPODU,
	C_XIANLUSUDU,
	C_FENXIANGQU,
	C_CHEZHAN,
    C_GUIDAODIANLU,
	C_LINSHIXIANSU,
    C_LICHENG
};


//********表格数据类型*********************
enum FileType
{
    JINLU = 0,
    CHEZHAN,
    PODU,
    GUOFENXIANG,
    SHUJU,
    SUDU,
    YINGDAQI,
    DUANLIAN,
    ZUOBIAOXI,
    LICHENG,
    ZHANTAI
};

//********列车运行方向*********************
enum Dir
{
    UPFRO = 0,                   //上行正向
    UPBAC,                       //上行反向
    DOWNFRO,                     //下行正向
    DOWNBAC                      //下行反向
};

enum Line_Dir
{
	UNKNOWN_Line_Dir = -1,               //未知
	UP = 0,                   //上行
	DOWN = 1                  //下行
};

enum Train_Dir
{
	UNKNOWN_Train_Dir = -1,               //未知
	BACK = 0,                   //反向
	FRONT = 1                     //正向
};

//轨道电路载频
enum Fre
{
    NO_CODE = 0,
    L3,
    L,
    L2,
    LU,
    U2,
    LU2,
    U,
    UU,
    UUS,
    U2S,
    L5,
    U3,
    L4,
    HB,
    F257,
    HU,
    F279,
    H,
    UNKNOW
};

enum Balish_Category
{
	JZ = 0,
	FCZ = 1,
	CZ = 2,
	FJZ = 3,
	DW = 4,
	FDW = 5,
	Q = 6,
	FQ = 7,
	ZJ1 = 8,
	FZJ1 = 9,
	ZJ2 = 10,
	FZJ2 = 11,
	UNKNOWN_Balish_Category = -1
};

//******进路信息***************************
typedef struct
{
    QString ID;                  //序号
    QString baliseID;            //应答器编号
    QString routeID;             //进路编号
    QString route;               //进路
    QString routeType;           //进路类型
    QString annBeginName;        //始端信号机名称
    QString annBeginCodeOrderMax;//始端信号机最高码序
    QString annunciatorEndName;  //终端信号机名称
    QString balise;              //应答器
    QString turnout;             //道岔
    QString speed;               //线路速度
    QString pathWay;             //轨道区段
    QString disasterPreLen;      //灾害防护区段
    QString remark;              //备注
	
}AccessRode;

typedef struct AccessRode_MapType_
{
	QVector<QString> balise_ID;        //链接的应答器
	QVector<int> balise_Diatance;  //到链接应答器的距离
	QVector<int> speed;                  //线路速度
	QVector<int> speed_Length;           //线路速度设定的长度
	QVector<int> pathWay_Length;         //轨道区段长度
	QVector<QString> pathWay_Fre;			//轨道区段载频
	QVector<QString> pathWay_SignalType;		//轨道区段信号机类型
	QVector<QString> pathWay_Name;			//轨道区段名称
	QString annBeginName;                   //始端信号机名称
	QString annEndName;                   //终端信号机名称


    void clear()
    {
        balise_ID.clear();
        balise_Diatance.clear();
        speed.clear();
        speed_Length.clear();
        pathWay_Length.clear();
        pathWay_Fre.clear();
        pathWay_SignalType.clear();
        pathWay_Name.clear();
        annBeginName.clear();
        annEndName.clear();
    }

    bool isEmpty()
    {
        if(!balise_ID.isEmpty())return false;
        if(!balise_Diatance.isEmpty())return false;
        if(!speed.isEmpty())return false;
        if(!speed_Length.isEmpty())return false;
        if(!pathWay_Length.isEmpty())return false;
        if(!pathWay_Fre.isEmpty())return false;
        if(!pathWay_SignalType.isEmpty())return false;
        if(!pathWay_Name.isEmpty())return false;
        if(!annBeginName.isEmpty())return false;
        if(!annEndName.isEmpty())return false;
        return true;
    }
}AccessRode_MapType;

typedef struct
{
	QString sheet_name;
	QVector<AccessRode> sheet;
}AccessRode_Sheet;

//******车站名*******************************
typedef struct
{
    QString ID;                  //序号
    QString stationName;         //车站名
    QString regionID;            //大区编号
    QString subareaID;           //分区编号
    QString stationID;           //车站编号
}Station;

//*******坡度********************************
typedef struct
{
    QString ID;                  //序号
    QString grade;               //坡度
    QString len;                 //长度
    QString endKm;               //终端里程
    QString remark;              //备注
}Grade;

//*******分相区*******************************
typedef struct
{
    QString ID;                  //序号
    QString beganKm;             //起始里程
    QString endKm;               //终点里程
    QString len;                 //长度（m）
    QString remark;              //备注
}NeutralSection;

//*****轨道区段数据****************************
typedef struct
{
    QString ID;                  //序号
    QString stationName;         //车站名
    QString signalSpotName;      //信号点-名称
    QString signalSpotKm;        //信号点-公里标
    QString signalSpotType;      //信号点-类型
    QString insulationJointType; //信号点-绝缘节类型
    QString pathWayName;         //轨道区段-名称
    QString pathWayFre;          //轨道区段-载频
    QString pathWayLen;          //轨道区段-长度
    QString pathWayCof;          //轨道区段-属性
    QString remark;              //备注
}PathWayData;

//****侧线轨道区段******************************
struct PathWayData_Less
{
	QString signalSpotType;
	QString pathWayFre;
	quint32 pathWayLen;
	PathWayData_Less() {};
	PathWayData_Less(QString signalSpotName_, QString pathWayFre_, int pathWayLen_):signalSpotType(signalSpotName_), pathWayFre(pathWayFre_), pathWayLen(pathWayLen_){};
};

//****线路速度**********************************
typedef struct
{
    QString ID;                  //序号
    QString speed;               //速度（km/h）
    QString len;                 //长度（m）
    QString endKm;               //终点里程
    QString remark;              //备注
}PathWaySpeed;

//****应答器位置********************************
typedef struct
{
    QString ID;                  //序号
    QString baliseName;          //应答器名称
    QString baliseID;            //应答器编号
    QString baliseKm;            //里程
    QString baliseType;          //设备类型
    QString baliseUse;           //用途
    QString remark_1;            //备注1
    QString remark_2;            //备注2
}BaliseLocation;

//*****断链明细*********************************
typedef struct
{
    QString lineName;            //线名
    QString lineType;            //线别
    QString brokenLinkType;      //断链类型
    QString brokenLinkPosSta;    //断链位置起点里程
    QString brokenLinkPosEnd;    //断链位置终点里程
    QString brokenLinkLongLen;   //断链长度-长链
    QString brokenLinkShortLen;  //断链长度-短链
}BrokenLink;

//*****坐标系信息********************************
typedef struct
{
    QString ID;                  //序号
    QString number;              //编号
    QString lineType;            //行别
    QString coordinateName;      //坐标系名称
    QString length;              //长度
    QString remark;              //备注
    QString isReverse;           //是否坐标系方向反向
    QString coordinateKm;        //本坐标系公里标
    QString nextCoordinateKm;    //临界坐标系公里标
    QString kmNumber;            //里程标系值   递增/递减
    QString kmTrend;             //里程趋势
    QString kmName;              //里程标系名称
}Coordinate;

//*****里程系信息表********************************
typedef struct
{
    QString ID;                   //序号
    QString beforeLineType;       //变换前行别
    QString beforeKm;             //变换前里程系公里标
    QString afterLineType;        //变换后行别
    QString afterType;            //变换后里程系公里标
    QString remark;               //备注
}KmInfo;

//*****站台侧信息表*********************************
typedef struct
{
    QString ID;                   //序号
    QString stationName;          //所属车站
    QString trackName;            //股道名称
    QString baliseName;           //描述站台侧信息的应答器名称
    QString stationSide;              //站台侧信息
}StationSide;


enum {
	Compare_Result = 0,
	Balish_BitMsg = 1,
	Compare_Result_BalishLocation =2,
	Compare_Result_TrackCircuit = 3,
	Compare_Result_Speed = 4,
	Compare_Result_TrackStation = 5,
	Compare_Result_Grade = 6,
	Compare_Result_NeutralSection = 7,
	Compare_Result_TrackTemporarySpeedLimit = 8,
	RBC_BitMsg = 9
};
QDataStream& operator<<(QDataStream& QDS, const AccessRode& C);
QDataStream& operator>>(QDataStream& QDS, AccessRode& C);
QDataStream& operator<<(QDataStream& QDS, const AccessRode_MapType& C);
QDataStream& operator>>(QDataStream& QDS, AccessRode_MapType& C);
QDataStream& operator<<(QDataStream& QDS, const AccessRode_Sheet& C);
QDataStream& operator>>(QDataStream& QDS, AccessRode_Sheet& C);
QDataStream& operator<<(QDataStream& QDS, const Station& C);
QDataStream& operator>>(QDataStream& QDS, Station& C);
QDataStream& operator<<(QDataStream& QDS, const Grade& C);
QDataStream& operator>>(QDataStream& QDS, Grade& C);
QDataStream& operator<<(QDataStream& QDS, const NeutralSection& C);
QDataStream& operator>>(QDataStream& QDS, NeutralSection& C);
QDataStream& operator<<(QDataStream& QDS, const PathWayData& C);
QDataStream& operator>>(QDataStream& QDS, PathWayData& C);
QDataStream& operator<<(QDataStream& QDS, const PathWayData_Less& C);
QDataStream& operator>>(QDataStream& QDS, PathWayData_Less& C);
QDataStream& operator<<(QDataStream& QDS, const PathWaySpeed& C);
QDataStream& operator>>(QDataStream& QDS, PathWaySpeed& C);
QDataStream& operator<<(QDataStream& QDS, const BaliseLocation& C);
QDataStream& operator>>(QDataStream& QDS, BaliseLocation& C);
QDataStream& operator<<(QDataStream& QDS, const BrokenLink& C);
QDataStream& operator>>(QDataStream& QDS, BrokenLink& C);
QDataStream& operator<<(QDataStream& QDS, const KmInfo& C);
QDataStream& operator>>(QDataStream& QDS, KmInfo& C);
QDataStream& operator<<(QDataStream& QDS, const StationSide& C);
QDataStream& operator>>(QDataStream& QDS, StationSide& C);