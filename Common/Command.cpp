#include "Command.h"

QDataStream &operator<<(QDataStream &QDS, const  ReportDataStructure &R)
{
    QDS << R.ATPTimestamp;
    QDS << R.MAINT_POS_GEO_EDIT;
    QDS << R.IsBalishDATA;
    QDS << static_cast<qint32>(R.Direction);
    QDS << R.BalishData;
    QDS << R.RBC;
    QDS << R.Compare_Result;
    return QDS;
}

QDataStream &operator>>(QDataStream &QDS, ReportDataStructure &R)
{
    QDS >> R.ATPTimestamp;
    QDS >> R.MAINT_POS_GEO_EDIT;
    QDS >> R.IsBalishDATA;
    qint32 direction;
    QDS >> direction;
    R.Direction = static_cast<Train_Dir>(direction);
    QDS >> R.BalishData;
    QDS >> R.RBC;
    QDS >> R.Compare_Result;
    return QDS;
}

QDataStream &operator<<(QDataStream &QDS, const BaliseBitMessage &Balish) {
    for (int i = 0; i < 104; i++)
        QDS << Balish.Balise_Tel[i];
    QDS << Balish.Locspace;
    QDS << Balish.Loctime;
    QDS << (static_cast<quint32>(Balish.type));
    return QDS;
}

QDataStream &operator>>(QDataStream &QDS, BaliseBitMessage &Balish) {
    for (int i = 0; i < 104; i++)
        QDS >> Balish.Balise_Tel[i];
    QDS >> Balish.Locspace;
    QDS >> Balish.Loctime;
    quint32 type;
    QDS >> type;
    Balish.type = static_cast<BaliseType>(type);
    return QDS;
}

QDataStream &operator<<(QDataStream &QDS, const  TcpHead &T) {
    QDS << T.cmd_from;
    QDS << T.cmd_type;
    QDS << T.cmd_name;
    return QDS;
}

QDataStream &operator<<(QDataStream &QDS, const  RealTimeDatastructure &R) {
    QDS << R.ATPTimestamp;//7*<8对应ATPItem_Public[11]-ATPItem_Public[16]表示数据时间
    QDS << R.IPCTimestamp;
    //ATP基础数据，DMS数据和200C数据的共同部分
    QDS << R.Has_DMS_ATP_DATA;
    if(R.Has_DMS_ATP_DATA)
    {
        QDS << R.Car_ID;//8*8车次号ATPItem_Public[22-29].value
        QDS << R.CTCS_LEVEL_EDIT;//3当前列控等级,ATPItem_Public[6].value
        QDS << R.CTCS_MODE_EDIT;//8当前控车模式,ATPItem_Public[7].value
        QDS << R.BREAK_COMMAND;//1,1,1,1须合并，EB,ATPItem_Public[9].value,SB7,ATPItem_Public[10].value,SB1，ATPItem_Public[34].value,SB4，ATPItem_Public[35].value
        QDS << R.MAINT_VIT_TRAIN_LCD;//16列车当前速度，对应ATPItem_Public[1].value
        QDS << R.MAINT_KV_VIT_CTRL_FU;//16EBP速度，对应ATPItem_Public[33].value
        QDS << R.MAINT_VIT_PRE_INTERV_LCD;//16当前干预速度,ATPItem_Public[18].value
        QDS << R.MAINT_VIT_OBJECTIF_LCD;//16当前目标速度,ATPItem_Public[19].value
        QDS << R.MAINT_DIS_OBJECTIF;//32目标距离，ATPItem_Public[32].value
        QDS << R.MAINT_POS_GEO_EDIT;//32当前里程,ATPItem_Public[30].value
        QDS << R.CTCS_CODE_EDIT;//8当前轨道电路载频,ATPItem_Public[20].value
        QDS << R.CTCS_CARRIER_EDIT;//8当前轨道电路低频,ATPItem_Public[21].value
        QDS << R.CTCS_DRIVER_ID;//16+16司机ID_1,ATPItem_Public[36].value，ATPItem_Public[37].value
    }
    //200c数据
    QDS << R.Has_200C_DATA;
    if(R.Has_200C_DATA)
    {
        QDS << R.m_MAIN_POS_TRAIN_LCD;//32列车自走行距离，对应ATPItem_Public[0].value
        QDS << R.MAINT_VIT_PERMISE_LCD;//列车当前允许速度，对应ATPItem_Public[2].value
        QDS << R.CTCS_TRAIN_LENGTH_EDIT;//车长,对应ATPItem_Public[3].value
        QDS << R.MAINT_L_DOUBTUNDER_EDIT;//16最大车头误差,对应ATPItem_Public[4].value
        QDS << R.MAINT_L_DOUBTOVER_EDIT;//16最小车头误差,对应ATPItem_Public[5].value
        QDS << R.CTCS_D_EOA_EDIT;//32当前MA长度,ATPItem_Public[8].value
        QDS << R.runLen;//32列车当前位置,cm,ATPItem_Public[0].value
        QDS << R.CTCS_L_TSR_EDIT;//32临时限速范围的剩余长度,ATPItem_CTCS_2[0].value
        QDS << R.Balish_ID;//10+14ATPItem_ETCS_5[0].value,ATPItem_ETCS_5[2].value
        QDS << R.CTCS_Text_Code;//8DMI文本显示,对应ATPItem_Public[17].value
    }
    //RBC数据
    QDS << R.Has_DMS_RBC_DATA;
    if(R.Has_DMS_RBC_DATA)
        QDS << R.RBC;
    //应答器报文
    QDS << R.Has_Balish_Compare_Result;
    if(R.Has_Balish_Compare_Result)
    {
        QDS << R.BalishData;
        QDS << R.Compare_Result;
    }
    return QDS;
}

QDataStream &operator>>(QDataStream &QDS, TcpHead &T) {
    QDS >> T.cmd_from;
    QDS >> T.cmd_type;
    QDS >> T.cmd_name;
    return QDS;
}

QDataStream &operator>>(QDataStream &QDS, RealTimeDatastructure &R) {
    QDS >> R.ATPTimestamp;//7*<8对应ATPItem_Public[11]-ATPItem_Public[16]表示数据时间
    QDS >> R.IPCTimestamp;
    //ATP基础数据，DMS数据和200C数据的共同部分
    QDS >> R.Has_DMS_ATP_DATA;
    if(R.Has_DMS_ATP_DATA)
    {
        QDS >> R.Car_ID;//8*8车次号ATPItem_Public[22-29].value
        QDS >> R.CTCS_LEVEL_EDIT;//3当前列控等级,ATPItem_Public[6].value
        QDS >> R.CTCS_MODE_EDIT;//8当前控车模式,ATPItem_Public[7].value
        QDS >> R.BREAK_COMMAND;//1,1,1,1须合并，EB,ATPItem_Public[9].value,SB7,ATPItem_Public[10].value,SB1，ATPItem_Public[34].value,SB4，ATPItem_Public[35].value
        QDS >> R.MAINT_VIT_TRAIN_LCD;//16列车当前速度，对应ATPItem_Public[1].value
        QDS >> R.MAINT_KV_VIT_CTRL_FU;//16EBP速度，对应ATPItem_Public[33].value
        QDS >> R.MAINT_VIT_PRE_INTERV_LCD;//16当前干预速度,ATPItem_Public[18].value
        QDS >> R.MAINT_VIT_OBJECTIF_LCD;//16当前目标速度,ATPItem_Public[19].value
        QDS >> R.MAINT_DIS_OBJECTIF;//32目标距离，ATPItem_Public[32].value
        QDS >> R.MAINT_POS_GEO_EDIT;//32当前里程,ATPItem_Public[30].value
        QDS >> R.CTCS_CODE_EDIT;//8当前轨道电路载频,ATPItem_Public[20].value
        QDS >> R.CTCS_CARRIER_EDIT;//8当前轨道电路低频,ATPItem_Public[21].value
        QDS >> R.CTCS_DRIVER_ID;//16+16司机ID_1,ATPItem_Public[36].value，ATPItem_Public[37].value
    }
    //200c数据
    QDS >> R.Has_200C_DATA;
    if(R.Has_200C_DATA)
    {
        QDS >> R.m_MAIN_POS_TRAIN_LCD;//32列车自走行距离，对应ATPItem_Public[0].value
        QDS >> R.MAINT_VIT_PERMISE_LCD;//列车当前允许速度，对应ATPItem_Public[2].value
        QDS >> R.CTCS_TRAIN_LENGTH_EDIT;//车长,对应ATPItem_Public[3].value
        QDS >> R.MAINT_L_DOUBTUNDER_EDIT;//16最大车头误差,对应ATPItem_Public[4].value
        QDS >> R.MAINT_L_DOUBTOVER_EDIT;//16最小车头误差,对应ATPItem_Public[5].value
        QDS >> R.CTCS_D_EOA_EDIT;//32当前MA长度,ATPItem_Public[8].value
        QDS >> R.runLen;//32列车当前位置,cm,ATPItem_Public[0].value
        QDS >> R.CTCS_L_TSR_EDIT;//32临时限速范围的剩余长度,ATPItem_CTCS_2[0].value
        QDS >> R.Balish_ID;//10+14ATPItem_ETCS_5[0].value,ATPItem_ETCS_5[2].value
        QDS >> R.CTCS_Text_Code;//8DMI文本显示,对应ATPItem_Public[17].value
    }
    //RBC数据
    QDS >> R.Has_DMS_RBC_DATA;
    if(R.Has_DMS_RBC_DATA)
        QDS >> R.RBC;
    //应答器报文
    QDS >> R.Has_Balish_Compare_Result;
    if(R.Has_Balish_Compare_Result)
    {
        QDS >> R.BalishData;
        QDS >> R.Compare_Result;
    }
    return QDS;
}

QDataStream &operator<<(QDataStream &QDS, const SequenceDataStructure &R)
{
	QDS << R.sequenceNumber;
	QDS << R.reportData;
	return QDS;
}

QDataStream &operator>>(QDataStream &QDS, SequenceDataStructure &R)
{
	QDS >> R.sequenceNumber;
	QDS >> R.reportData;
	return QDS;
}

QByteArray Combine_Command_Data(TcpHead head)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_6);
    quint32 size = 0;
    out << size;
    out << head;
    out.device()->seek(0);
    out << static_cast<quint32>(quint32(static_cast<quint32>(block.size()) - sizeof(quint32)));
    return block;
}

