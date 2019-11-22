#pragma once
#include "qobject.h"
#include <QDataStream>
#include "basestructer.h"
#include "self_define.h"

enum CMD_FROM
{
    RECEIVER = 0xf0,
    CLIENT = 0x0f,
    WORKER = 0xaa,
};

enum CMD_TYPE
{
    CONTROL = 0x01,
    DATA = 0x02,
    INSIDE = 0x03,
};

enum CMD_NAME
{
    UNUSED_NAME,//默认
	DATA_SHEET,//更改工程数据表
	REALTIME_DATA,//发线别送实时数据，有应答器时包含应答器数据
	DMS_ATP,//常DMS规数据中的ATP数据
	DMS_Balish,//常规DMS数据中的应答器报文数据
	DMS_RBC,//常规DMS数据中的RBC报文数据
	DMS_REPORT,//數據是用于寫入報告文件裏的
	SEND_TRAIN_NUMBER,//发送当前车次号
	SET_TRAIN_NUMBER,//更改当前车次号
	LIST_FILE,//列出所有存储的文件
	TRANSFER_REPORT,//传送文件信息
	GET_SELECTED_FILE,//传输指定文件给客户端
	DELETE_SELECTED_FILE,//删除指定的文件
	BETTERY_PERSENT,//电量信息
    SOCKET_TYPE,//用于确定连接的socket的类型
    SD_PERSENT,
};

enum SOCKET_TYPE {
    COMMAND_SOCKET = 1,
    DATA_SOCKET = 2
};

typedef struct
{
    //必有的数据
    qint64 ATPTimestamp=0;//7*<8对应ATPItem_Public[11]-ATPItem_Public[16]表示数据时间
    qint64 IPCTimestamp=0;//数据到达的接收时间
    //ATP基础数据，DMS数据和200C数据的共同部分
    bool Has_DMS_ATP_DATA = false;
    quint64 Car_ID=0;//8*8车次号ATPItem_Public[22-29].value
    quint8 CTCS_LEVEL_EDIT=0;//3当前列控等级,ATPItem_Public[6].value
    quint8 CTCS_MODE_EDIT=0;//8当前控车模式,ATPItem_Public[7].value
    quint8 BREAK_COMMAND=0;//1,1,1,1须合并，EB,ATPItem_Public[9].value,SB7,ATPItem_Public[10].value,SB1，ATPItem_Public[34].value,SB4，ATPItem_Public[35].value
    quint16 MAINT_VIT_TRAIN_LCD=0;//16列车当前速度，对应ATPItem_Public[1].value
    quint16 MAINT_KV_VIT_CTRL_FU=0;//16EBP速度，对应ATPItem_Public[33].value
    quint16 MAINT_VIT_PRE_INTERV_LCD=0;//16当前干预速度,ATPItem_Public[18].value
    quint16 MAINT_VIT_OBJECTIF_LCD=0;//16当前目标速度,ATPItem_Public[19].value
    quint32 MAINT_DIS_OBJECTIF=0;//32目标距离，ATPItem_Public[32].value
    quint32 MAINT_POS_GEO_EDIT=0;//32当前里程,ATPItem_Public[30].value
    quint8 CTCS_CODE_EDIT=0;//8当前轨道电路载频,ATPItem_Public[20].value
    quint8 CTCS_CARRIER_EDIT=0;//8当前轨道电路低频,ATPItem_Public[21].value
    quint32 CTCS_DRIVER_ID=0;//16+16司机ID_1,ATPItem_Public[36].value，ATPItem_Public[37].value
    //200c数据独有部分
    bool Has_200C_DATA = false;
    quint32 m_MAIN_POS_TRAIN_LCD=0;//32列车自走行距离，对应ATPItem_Public[0].value
    quint16 MAINT_VIT_PERMISE_LCD=0;//列车当前允许速度，对应ATPItem_Public[2].value
    quint8 CTCS_TRAIN_LENGTH_EDIT=0;//车长,对应ATPItem_Public[3].value
    quint16 MAINT_L_DOUBTUNDER_EDIT=0;//16最大车头误差,对应ATPItem_Public[4].value
    quint16 MAINT_L_DOUBTOVER_EDIT=0;//16最小车头误差,对应ATPItem_Public[5].value
    quint32 CTCS_D_EOA_EDIT=0;//32当前MA长度,ATPItem_Public[8].value
    quint32 runLen=0;//32列车当前位置,cm,ATPItem_Public[0].value
    quint32 CTCS_L_TSR_EDIT=0;//32临时限速范围的剩余长度,ATPItem_CTCS_2[0].value
    quint32 Balish_ID=0;//10+14ATPItem_ETCS_5[0].value,ATPItem_ETCS_5[2].value
    quint8 CTCS_Text_Code=0;//8DMI文本显示,对应ATPItem_Public[17].value
    //RBC数据，DMS数据独有部分
    bool Has_DMS_RBC_DATA = false;
    QByteArray RBC;
    //应答器报文
    bool Has_Balish_Compare_Result = false;
    BaliseBitMessage BalishData;
    QVector<QVector<QVector<QString>>> Compare_Result;
}RealTimeDatastructure;


typedef struct
{
    qint64 ATPTimestamp = 0;//7*<8对应ATPItem_Public[11]-ATPItem_Public[16]表示数据时间
    quint32 MAINT_POS_GEO_EDIT = 0;//32当前里程,ATPItem_Public[30].value
    bool IsBalishDATA = false;
    Train_Dir Direction = Train_Dir::UNKNOWN_Train_Dir;
    BaliseBitMessage BalishData;
    QByteArray RBC;
    QVector<QVector<QVector<QString>>> Compare_Result;
}ReportDataStructure;

typedef struct
{
	QString sequenceNumber;
	QVector<ReportDataStructure> reportData;
}SequenceDataStructure;

class TcpHead
{
public:
    quint8 cmd_from;
    quint8 cmd_type;
    quint8 cmd_name;
    TcpHead() = default;
    TcpHead(CMD_FROM cmd_from, CMD_TYPE cmd_type, CMD_NAME cmd_name)
    {
        this->cmd_from = cmd_from;
        this->cmd_type = cmd_type;
        this->cmd_name = cmd_name;
    }
};

QDataStream &operator<<(QDataStream &QDS, const BaliseBitMessage &Balish);
QDataStream &operator>>(QDataStream &QDS, BaliseBitMessage &Balish);
QDataStream &operator<<(QDataStream &QDS, const TcpHead &T);
QDataStream &operator>>(QDataStream &QDS, TcpHead &T);
QDataStream &operator<<(QDataStream &QDS, const RealTimeDatastructure &R);
QDataStream &operator>>(QDataStream &QDS, RealTimeDatastructure &R);
QDataStream &operator<<(QDataStream &QDS, const ReportDataStructure &R);
QDataStream &operator>>(QDataStream &QDS, ReportDataStructure &R);
QDataStream &operator<<(QDataStream &QDS, const SequenceDataStructure &R);
QDataStream &operator>>(QDataStream &QDS, SequenceDataStructure &R);

//可变模板参数的函数定义以及其展开规则定义
static void PutIntoDataStream(QDataStream&)
{
    return;
}
template<class T, class... Args>
static void PutIntoDataStream(QDataStream& out, T& first, Args& ... rest)
{
	out << first;
	PutIntoDataStream(out, rest...);
}
template<class T, class... Args>
QByteArray Combine_Command_Data(TcpHead head, const T& first, const Args& ... rest)
{
	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_5_6);
	quint32 size = 0;
	out << size;
	out << head;
	PutIntoDataStream(out, first, rest...);
	out.device()->seek(0);
	out << static_cast<quint32>(quint32(static_cast<quint32>(block.size()) - sizeof(quint32)));
	return block;
}
