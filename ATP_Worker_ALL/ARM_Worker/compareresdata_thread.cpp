#include "compareresdata_thread.h"
#include "basestructer.h"
#include <math.h>
#include <QDateTime>
#include <strstream>
#include <QDebug>
#include <QTextCodec>
#include <QStack>
#include <iostream>
#include <QString>
#include "inner_tcp_client.h"
#include <QtAlgorithms>
#include <QMutexLocker>
#include "baliselocation.h"
#include "balisestation.h"
using namespace std;

//初始化静态成员
Train_Dir g_TrainDirection;//列车运行方向
Line_Dir g_LineDirection;//列车运行线别

//********待分离变量***********************/
int firstSpecialLen = 30;   //比对轨道电路
int secondSpecialLen = 230;


//QQueue<Record_Baliseposition> Record_Baliseposition_Collector;


CompareBaliseData_THREAD::CompareBaliseData_THREAD(QObject *parent) : QObject(parent),
Balish_Category2Str{ {JZ,"JZ"},{FCZ,"FCZ"},{CZ,"CZ"},{FJZ,"FJZ"},{DW,"DW"},{FDW,"FDW"},{Q,"Q"},{FQ,"FQ"},{ZJ1,"ZJ1"},{FZJ1,"FZJ1"},{ZJ2,"ZJ2"},{FZJ2,"FZJ2"},{UNKNOWN_Balish_Category,"UNKNOWN_Balish_Category"} }
{
    resultVec.resize(8);

	g_TrainDirection = BACK;
	g_LineDirection = DOWN;

    connect(G_innerTcpClient, SIGNAL(UdpDataCome_SIGNAL(qint32, qint64)), this, SLOT(on_UdpThread_DataCome(qint32, qint64)));
    connect(G_innerTcpClient, SIGNAL(DMSDataCome_SIGNAL(QByteArray, quint8)), this, SLOT(on_DMSTcp_DataCome(QByteArray, quint8)));
}


void CompareBaliseData_THREAD::FillRTD_by_DMSData(RealTimeDatastructure &RTD, QByteArray &datastream, quint8 DMStype)
{
	if (DMStype > 3 || DMStype < 1)
	{
		qDebug() << "FillRTD_by_DMSData()函数中数据类型未知";
		return;
	}
	int year = ((quint8(datastream[0]) >> 4) * 10 + (quint8(datastream[0]) & 0x0F)) * 100 + ((quint8(datastream[1]) >> 4) * 10 + (quint8(datastream[1]) & 0x0F));
	int month = (quint8(datastream[2]) >> 4) * 10 + (quint8(datastream[2]) & 0x0F);
	int day = (quint8(datastream[3]) >> 4) * 10 + (quint8(datastream[3]) & 0x0F);
	int hour = (quint8(datastream[4]) >> 4) * 10 + (quint8(datastream[4]) & 0x0F);
	int minute = (quint8(datastream[5]) >> 4) * 10 + (quint8(datastream[5]) & 0x0F);
	int second = (quint8(datastream[6]) >> 4) * 10 + (quint8(datastream[6]) & 0x0F);
	int millionsecond = ((quint8(datastream[7]) >> 4) * 10 + (quint8(datastream[7]) & 0x0F)) * 10;
	QDate QD(year, month, day);
	QTime QT(hour, minute, second, millionsecond);
	QDateTime QDT(QD, QT);

	RTD.ATPTimestamp = QDT.toMSecsSinceEpoch();//时间戳
	static qint64 lastTimeStamp = 0;
	while (QDateTime::currentMSecsSinceEpoch() == lastTimeStamp);
	lastTimeStamp = QDateTime::currentMSecsSinceEpoch();
	RTD.IPCTimestamp = lastTimeStamp;
	if (DMStype == 1)//ATP基础数据
	{
		//必填内容
		RTD.Has_DMS_RBC_DATA = false;
		RTD.Has_200C_DATA = false;
		RTD.Has_DMS_ATP_DATA = true;
		RTD.Has_Balish_Compare_Result = false;
		RTD.Car_ID = (quint64(quint8(datastream[8])) << 56) | (quint64(quint8(datastream[9])) << 48) | (quint64(quint8(datastream[10])) << 40) | (quint64(quint8(datastream[11])) << 32) | (quint64(quint8(datastream[12])) << 24) | (quint64(quint8(datastream[13])) << 16) | (quint64(quint8(datastream[14])) << 8) | (quint64(quint8(datastream[15])));
		RTD.CTCS_LEVEL_EDIT = quint8(datastream[16]);
		RTD.CTCS_MODE_EDIT = quint8(datastream[17]);
		RTD.BREAK_COMMAND = quint8(datastream[18]);
		if (RTD.BREAK_COMMAND == 253)
		{
			qDebug() << "本次的接收数据为：";
			for (auto iter = datastream.begin(); iter != datastream.end(); iter++)
			{
				std::cout << QString("%1").arg((unsigned char)*iter, 2, 16, QChar('0')).toStdString() << " ";
			}
			std::cout << std::endl;
		}
		RTD.MAINT_VIT_TRAIN_LCD = (quint8(datastream[20]) << 8) + quint8(datastream[19]);
		RTD.MAINT_KV_VIT_CTRL_FU = (quint8(datastream[22]) << 8) + quint8(datastream[21]);
		RTD.MAINT_VIT_PRE_INTERV_LCD = (quint8(datastream[24]) << 8) + quint8(datastream[23]);
		RTD.MAINT_VIT_OBJECTIF_LCD = (quint8(datastream[26]) << 8) + quint8(datastream[25]);
		RTD.MAINT_DIS_OBJECTIF = (quint64(quint8(datastream[30]) << 24)) | quint64((quint8(datastream[29]) << 16)) | quint64((quint8(datastream[28]) << 8)) | quint64(quint8(datastream[27]));
		RTD.MAINT_POS_GEO_EDIT = (quint64(quint8(datastream[34]) << 24)) | quint64((quint8(datastream[33]) << 16)) | quint64((quint8(datastream[32]) << 8)) | quint64(quint8(datastream[31]));
		RTD.CTCS_CODE_EDIT = quint8(datastream[35]);
		RTD.CTCS_CARRIER_EDIT = quint8(datastream[36]);
		RTD.CTCS_DRIVER_ID = quint64((quint8(datastream[40]) << 24)) | quint64((quint8(datastream[39]) << 16)) | quint64((quint8(datastream[38]) << 8)) | quint64(quint8(datastream[37]));
	}
	else if (DMStype == 2)//应答器数据,该项必须在执行完数据比对函数之后填充
	{
		//必填内容
		RTD.Has_DMS_RBC_DATA = false;
		RTD.Has_200C_DATA = false;
		RTD.Has_DMS_ATP_DATA = false;
		RTD.Has_Balish_Compare_Result = true;
		RTD.BalishData = m_originBaliseBitMessage;
		RTD.Compare_Result = resultVec;
	}
	else if (DMStype == 3)//RBC数据
	{
		//必填内容
		RTD.Has_DMS_RBC_DATA = true;
		RTD.Has_200C_DATA = false;
		RTD.Has_DMS_ATP_DATA = false;
		RTD.Has_Balish_Compare_Result = false;
		RTD.RBC = datastream.mid(8);
	}
}

void CompareBaliseData_THREAD::FillRTD_by_AMData(RealTimeDatastructure &RTD, Analyze_ATPMessage* AM, bool has_compare_result, qint64 timestamp)
{
	QDate QD(2000 + AM->ATPItem_Public[11].value, AM->ATPItem_Public[12].value, AM->ATPItem_Public[13].value);
	QTime QT(AM->ATPItem_Public[14].value, AM->ATPItem_Public[15].value, AM->ATPItem_Public[16].value, AM->ATPItem_Public[31].value * 50);
	QDateTime QDT(QD, QT);

	RTD.ATPTimestamp = QDT.toMSecsSinceEpoch();//时间戳
	RTD.IPCTimestamp = timestamp;
	//必填内容
	RTD.Has_DMS_RBC_DATA = false;
	RTD.Has_200C_DATA = true;
	RTD.Has_DMS_ATP_DATA = true;
	RTD.Has_Balish_Compare_Result = has_compare_result;
	//ATP基础数据
	RTD.Car_ID = (AM->ATPItem_Public[22].value << 56) | (AM->ATPItem_Public[23].value << 48) | (AM->ATPItem_Public[24].value << 40) | (AM->ATPItem_Public[25].value << 32) | (AM->ATPItem_Public[26].value << 24) | (AM->ATPItem_Public[27].value << 16) | (AM->ATPItem_Public[28].value << 8) | (AM->ATPItem_Public[29].value);
	RTD.CTCS_LEVEL_EDIT = AM->ATPItem_Public[6].value;
	RTD.CTCS_MODE_EDIT = AM->ATPItem_Public[7].value;
	if (AM->ATPItem_Public[9].value)
		RTD.BREAK_COMMAND = 13;
	else if (AM->ATPItem_Public[10].value)
		RTD.BREAK_COMMAND = 7;
	else if (AM->ATPItem_Public[34].value)
		RTD.BREAK_COMMAND = 1;
	else if (AM->ATPItem_Public[35].value)
		RTD.BREAK_COMMAND = 4;
	else
		RTD.BREAK_COMMAND = 0;
	RTD.MAINT_VIT_TRAIN_LCD = AM->ATPItem_Public[1].value;
	RTD.MAINT_KV_VIT_CTRL_FU = AM->ATPItem_Public[33].value;
	RTD.MAINT_VIT_PRE_INTERV_LCD = AM->ATPItem_Public[18].value;
	RTD.MAINT_VIT_OBJECTIF_LCD = AM->ATPItem_Public[19].value;
	RTD.MAINT_DIS_OBJECTIF = AM->ATPItem_Public[32].value;
	RTD.MAINT_POS_GEO_EDIT = AM->ATPItem_Public[30].value;
	RTD.CTCS_CODE_EDIT = AM->ATPItem_Public[20].value;
	RTD.CTCS_CARRIER_EDIT = AM->ATPItem_Public[21].value;
	RTD.CTCS_DRIVER_ID = (AM->ATPItem_Public[36].value << 16) | AM->ATPItem_Public[37].value;
	//200c
	RTD.m_MAIN_POS_TRAIN_LCD = AM->ATPItem_Public[0].value;
	RTD.MAINT_VIT_PERMISE_LCD = AM->ATPItem_Public[2].value;
	RTD.CTCS_TRAIN_LENGTH_EDIT = AM->ATPItem_Public[3].value;
	RTD.MAINT_L_DOUBTUNDER_EDIT = AM->ATPItem_Public[4].value;
	RTD.MAINT_L_DOUBTOVER_EDIT = AM->ATPItem_Public[5].value;
	RTD.CTCS_D_EOA_EDIT = AM->ATPItem_Public[8].value;
	RTD.runLen = AM->ATPItem_Public[0].value;
	RTD.CTCS_L_TSR_EDIT = AM->ATPItem_CTCS_2[0].value;
	RTD.Balish_ID = (AM->ATPItem_ETCS_5[0].value << 14) | (AM->ATPItem_ETCS_5[2].value);
	RTD.CTCS_Text_Code = AM->ATPItem_Public[17].value;
	//应答器
	if (RTD.Has_Balish_Compare_Result)
	{
		RTD.BalishData = m_originBaliseBitMessage;
		RTD.Compare_Result = resultVec;
	}
}

//DMS数据的对比逻辑
void CompareBaliseData_THREAD::on_DMSTcp_DataCome(QByteArray datastream, quint8 type)
{
	switch (type)
	{
	case 1://ATP基础数据
	{
		FillRTD_by_DMSData(RTD, datastream, type);//填充RTD数据
        emit G_innerTcpClient->send(Combine_Command_Data(TcpHead(CMD_FROM::WORKER, CMD_TYPE::INSIDE, CMD_NAME::DMS_ATP), RTD));
		break;
	}
	case 2://应答器数据
	{
		memcpy(m_originBaliseBitMessage.Balise_Tel, datastream.data() + 8, 104);//应答器赋值
		m_originBaliseBitMessage.type = BaliseType::Normal;

		/***********************判断应答器类型************************************************/
		m_Balise_IDstr = Analyze_BaliseMessage::GetBaliseID(m_originBaliseBitMessage);//获取应答器标识号
		auto iter = DesignData::baliseLocationUpMap.find(m_Balise_IDstr);//上行表中查找
		if (iter == DesignData::baliseLocationUpMap.end())//没找到
			iter = DesignData::baliseLocationDownMap.find(m_Balise_IDstr);//下行表中查找
		if (iter != DesignData::baliseLocationDownMap.end())//如果两个表都没找到，则该迭代器等于下行表的尾后迭代器，不等说明找到了
			if (iter->baliseType == "有源")
				m_originBaliseBitMessage.type = (BaliseType)(m_originBaliseBitMessage.type | BaliseType::Active);
			else;
		else
			qDebug() << "connot find Balish ID:" << m_Balise_IDstr;
		qDebug() << "Balish ID:" << m_Balise_IDstr;
		/*****************************************************************************************/
		CompareBaliseData(1);
		FillRTD_by_DMSData(RTD, datastream, type);
        emit G_innerTcpClient->send(Combine_Command_Data(TcpHead(CMD_FROM::WORKER, CMD_TYPE::INSIDE, CMD_NAME::DMS_Balish), RTD));

		ReportDataStructure RDS;
        RDS.ATPTimestamp = RTD.IPCTimestamp;
		RDS.BalishData = m_originBaliseBitMessage;
		RDS.Direction = g_TrainDirection;
		RDS.Compare_Result = resultVec;
		RDS.IsBalishDATA = true;
		RDS.MAINT_POS_GEO_EDIT = RTD.MAINT_POS_GEO_EDIT;
		emit G_innerTcpClient->send(Combine_Command_Data(TcpHead(CMD_FROM::WORKER, CMD_TYPE::INSIDE, CMD_NAME::DMS_REPORT), RDS));
        //DMSReportFile << RDS;
		break;
	}
	case 3://RBC数据
	{
		FillRTD_by_DMSData(RTD, datastream, type);//填充RTD数据
        emit G_innerTcpClient->send(Combine_Command_Data(TcpHead(CMD_FROM::WORKER, CMD_TYPE::INSIDE, CMD_NAME::DMS_RBC), RTD));
		break;
	}
	default:
		assert(false);
		break;
	}
	static qint64 last = 0;
	last = RTD.IPCTimestamp;
	return;
}



/**********************************************************
 * @功能：udp数据到来时连接的槽函数，需对所有比对项进行对比，并将比对结果作为信号参数发送给主界面，主界面需添加比对历史和首页错误信息等
 * @形参：order-原始数据索引
 * @返回值：无
 * @备注：信号与槽连接关系见main.cpp
 * @作者：hb
 * @修改日期：
**********************************************************/
void CompareBaliseData_THREAD::on_UdpThread_DataCome(qint32 order, qint64 timestamp)
{
//	ATPBitData.clear();
//	ATPBitData = ETH_Udp::GetAtpBitMsg_InBinFile(ATPBit_File.getfile(), order);
//	MTX.lock();
//	_AM->AnalyzeGetMainInfo(ATPBitData);//提取本次的atp数据的解析内容
//	MTX.unlock();
//	int BalishNum = _AM->GetBelishMessage(ATPBitData, m_originBaliseBitMessage);
//	/***********************判断应答器类型************************************************/
//	if (BalishNum == 2)
//	{
//		m_Balise_IDstr = Analyze_BaliseMessage::GetBaliseID(m_originBaliseBitMessage);//获取应答器标识号
//		auto iter = DesignData::baliseLocationUpMap.find(m_Balise_IDstr);//上行表中查找
//		if (iter == DesignData::baliseLocationUpMap.end())//没找到
//			iter = DesignData::baliseLocationDownMap.find(m_Balise_IDstr);//下行表中查找
//		if (iter != DesignData::baliseLocationDownMap.end())//如果两个表都没找到，则该迭代器等于下行表的尾后迭代器，不等说明找到了
//			if (iter->baliseType == "有源")
//				m_originBaliseBitMessage.type = (BaliseType)(m_originBaliseBitMessage.type | BaliseType::Active);
//			else;
//		else
//			qDebug() << "在应答器位置表中未找到应答器,应答器编号:" << m_Balise_IDstr;
//	}
//	/*****************************************************************************************/
//	RealTimeDatastructure RTD;

//	TcpHead tcphead(CMD_FROM::IPC, CMD_TYPE::DATA, CMD_NAME::REALTIME_DATA);

//	QBA.clear();
//	QDataStream QDS(&QBA, QIODevice::WriteOnly);
//	QDS.setVersion(QDataStream::Qt_5_6);

//	if (BalishNum != 2)//没有应答器报文
//	{
//		FillRTD_by_AMData(RTD, _AM, false, timestamp);
//	}
//	else
//	{
//		CompareBaliseData(0);//进行比对
//		FillRTD_by_AMData(RTD, _AM, true, timestamp);
//	}
//	QDS << (qint32)0 << tcphead << RTD;//4byte占位字节用于描述信息长度
//	QDS.device()->seek(0);//指针跳回流的起始位置
//	QDS << (qint32)(QBA.size() - sizeof(qint32));//修改占位字节
//	emit TcpServer->SendToAll_SIGNAL(QBA);//发送给所有连接的客户端
//	return;
}

void CompareBaliseData_THREAD::CompareBaliseData(int DataType)
{
    QMutexLocker locker(&DesignData::SheetMutex);
    static QList<QString> RecordBaliseGroup;
	SideLineFlag = false;//侧线信息置空操作，如果侧线该变量不为空，则比对逻辑中会执行侧线的比对逻辑
	m_baliseData_Package = Analyze_BaliseMessage::Resolve(m_originBaliseBitMessage);//解析原始数据获得应答器报文的解析结果
	m_Balise_IDstr = Analyze_BaliseMessage::GetBaliseID(m_originBaliseBitMessage);//获取应答器标识号
	m_Balise_GroupID = m_Balise_IDstr;
	m_Balise_GroupID.resize(12);
    if(RecordBaliseGroup.isEmpty()||m_Balise_GroupID!=RecordBaliseGroup.back())
        RecordBaliseGroup.push_back(m_Balise_GroupID);
    if(RecordBaliseGroup.size()>10)
        RecordBaliseGroup.pop_front();

	New_BalishGroup = false;
	if (m_LastBalise_GroupID != m_Balise_GroupID)//应答器组内应答器ID相同，经过应答器组时进入判断
	{
		string std_Balise_GroupID = m_Balise_GroupID.toStdString();
		int m_Balise_GroupID_Number = std_Balise_GroupID[9] * 100 + std_Balise_GroupID[10] * 10 + std_Balise_GroupID[11];
		if (m_Balise_GroupID_Number % 2 == 0)
			g_LineDirection = UP;
		else
			g_LineDirection = DOWN;
		if (m_Balise_IDstr.size() == 14)
			g_TrainDirection = (m_Balise_IDstr[m_Balise_IDstr.size() - 1] == '1' ? FRONT : BACK);
		m_LastBalise_GroupID = m_Balise_GroupID;//更新上一次的应答器标识号
		New_BalishGroup = true;
		qDebug() << (g_LineDirection == UP ? "上行" : "下行") << (g_TrainDirection == FRONT ? "正向" : "反向");
	}

	QMap<QString, QString>& BalishUse = DesignData::balishUseMap;
	if (BalishUse.find(m_Balise_GroupID) != BalishUse.end())//数据表中可以查到应答器编号
	{
		if (BalishUse[m_Balise_GroupID].startsWith("JZ"))
			SideLineIndex = g_TrainDirection == FRONT ? Balish_Category::JZ : Balish_Category::FJZ;
		else if (BalishUse[m_Balise_GroupID].startsWith("FCZ"))
			SideLineIndex = g_TrainDirection == FRONT ? Balish_Category::FCZ : Balish_Category::CZ;
		else if (BalishUse[m_Balise_GroupID].startsWith("CZ"))
			SideLineIndex = g_TrainDirection == FRONT ? Balish_Category::CZ : Balish_Category::FCZ;
		else if (BalishUse[m_Balise_GroupID].startsWith("FJZ"))
			SideLineIndex = g_TrainDirection == FRONT ? Balish_Category::FJZ : Balish_Category::JZ;
		else if (BalishUse[m_Balise_GroupID].startsWith("DW"))
			SideLineIndex = g_TrainDirection == FRONT ? Balish_Category::DW : Balish_Category::FDW;
		else if (BalishUse[m_Balise_GroupID].startsWith("FDW"))
			SideLineIndex = g_TrainDirection == FRONT ? Balish_Category::FDW : Balish_Category::DW;
		else if (BalishUse[m_Balise_GroupID].startsWith("Q"))
			SideLineIndex = g_TrainDirection == FRONT ? Balish_Category::Q : Balish_Category::FQ;
		else if (BalishUse[m_Balise_GroupID].startsWith("FQ"))
			SideLineIndex = g_TrainDirection == FRONT ? Balish_Category::FQ : Balish_Category::Q;
		else if (BalishUse[m_Balise_GroupID].startsWith("ZJ1"))
			SideLineIndex = g_TrainDirection == FRONT ? Balish_Category::ZJ1 : Balish_Category::FZJ1;
		else if (BalishUse[m_Balise_GroupID].startsWith("ZJ2"))
			SideLineIndex = g_TrainDirection == FRONT ? Balish_Category::ZJ2 : Balish_Category::FZJ2;
		else
			SideLineIndex = Balish_Category::UNKNOWN_Balish_Category;
	}
	else
	{
		qDebug() << "con not find Balish ID：" + m_Balise_GroupID;
		SideLineIndex = Balish_Category::UNKNOWN_Balish_Category;
	}

	//在这里建立order与g_TrainDirection的映射，在on_clickHistoryInfo中可通过直接查表获取列车运行方向
	m_trainDirectionVec.append(g_TrainDirection);
	m_baliseLocation = DesignData::FindBaliseKmByID(m_Balise_IDstr);//应答器位置
																	//QVector<QVector<QString> >  resultVec[7];

	QVector<QVector<QString> > temp_result_vec;
	QVector<int> positionVec;
	for (int i = 0; i < 7; i++)
		resultVec[i].clear();

	//按顺序获取本应答器链接的所有应答器组的编号
    QList<Record_Baliseposition> AllLinkedBalishGroup = BaliseLocation::LinkedBliseGroupID(m_baliseData_Package);
	if (!AllLinkedBalishGroup.isEmpty())
		ALGI = AllLinkedBalishGroup;
	//更新下一组链接的应答器组
	NBGI = ALGI.isEmpty() ? "" : ALGI.front().ID;
	if (SideLineIndex == Balish_Category::JZ)//进站应答器更新进站数据
	{
		if (!NBGI.isEmpty())
		{
			QList<AccessRode_MapType> SideLineInfo_List = DesignData::accessRodeMap.values(m_Balise_GroupID);//把所有符合的应答器信息找出来
			for (auto iter = SideLineInfo_List.begin(); iter != SideLineInfo_List.end(); iter++)
			{
				if (iter->balise_ID.size())
				{
					if (iter->balise_ID[0] == NBGI)
					{
						qDebug() << "找到进站侧线";
						SideLineInfo = *iter;
						SideLineInfoIn = *iter;
						SideLineFlag = true;
						break;
					}
				}
			}
		}
		else
			qDebug() << (m_Balise_IDstr + "：NBGI为空，没有找到进站进路数据");
		if (AllLinkedBalishGroup.size() >= 2 && BalishUse.find(AllLinkedBalishGroup[1].ID) != BalishUse.end() && (BalishUse[AllLinkedBalishGroup[1].ID].startsWith("JZ") || BalishUse[AllLinkedBalishGroup[1].ID].startsWith("FJZ")))
		{
			QString StartGroupID = AllLinkedBalishGroup[0].ID;
			QString EndGroupID = AllLinkedBalishGroup[1].ID;
			QList<AccessRode_MapType> SideLineInfo_List = DesignData::accessRodeMap.values(StartGroupID);//把所有符合的应答器信息找出来
			for (auto iter = SideLineInfo_List.begin(); iter != SideLineInfo_List.end(); iter++)
			{
				if (iter->balise_ID.size())
				{
					if (iter->balise_ID[0] == EndGroupID)
					{
						qDebug() << "找到出站侧线";
						SideLineInfoOut = *iter;
						SideLineFlag = true;
						break;
					}
				}
			}
		}
	}
	else if (SideLineIndex == Balish_Category::FCZ || SideLineIndex == Balish_Category::CZ)
	{
		
		if (!NBGI.isEmpty())
		{
			QString StartGroupID;
			QString EndGroupID;
			if (BalishUse[NBGI].startsWith("FJZ") ||BalishUse[NBGI].startsWith("JZ"))
			{
				StartGroupID = m_Balise_GroupID;
				EndGroupID = NBGI;
			}
			else
			{
				if (!AllLinkedBalishGroup.isEmpty())
				{
					if (AllLinkedBalishGroup.size() >= 2)
					{
						StartGroupID = AllLinkedBalishGroup[0].ID;
						EndGroupID = AllLinkedBalishGroup[1].ID;
					}
					else
					{
						qDebug()<<(m_Balise_IDstr + "AllLinkedBalishGroup 少于两个");
						SideLineInfoOut.clear();
					}
				}
				else if (ALGI.size()>=2)
				{
					StartGroupID = ALGI[0].ID;
					EndGroupID = ALGI[1].ID;
				}
				else
				{
					qDebug() << (m_Balise_IDstr + "Record_Baliseposition_Collector 少于两个");
					SideLineInfoOut.clear();
				}
			}
			QList<AccessRode_MapType> SideLineInfo_List = DesignData::accessRodeMap.values(StartGroupID);//把所有符合的应答器信息找出来
			for (auto iter = SideLineInfo_List.begin(); iter != SideLineInfo_List.end(); iter++)
			{
				if (iter->balise_ID.size())
				{
					if (iter->balise_ID[0] == EndGroupID)
					{
						qDebug() << "找到出站侧线";
						SideLineInfo = *iter;
						SideLineInfoOut = *iter;
						SideLineFlag = true;
						break;
					}
				}
			}
		}
		else
		{
			qDebug() << (m_Balise_IDstr + "：NBGI为空，没有找到出站进路数据");
			SideLineInfoOut.clear();
		}
	}
	qDebug() << "SideLineIndex" << Balish_Category2Str[SideLineIndex];
    temp_result_vec = BaliseLocation::Compare(m_baliseData_Package,RecordBaliseGroup);
    if (temp_result_vec.size() != 0)
    {
        emit hasCompareResult_udp(temp_result_vec, C_YINGDAQIWEIZHI);
        resultVec[0] = temp_result_vec;
    }
    if(m_Balise_GroupID=="117-1-02-099")
        qDebug()<<"stop";
	positionVec = Analyze_BaliseMessage::GetPackagePosition("ETCS-44", m_baliseData_Package);
	for (int i = 0; i < positionVec.size(); i++)
	{
		temp_result_vec = Compare_TrackCircuit(positionVec[i]);
		if (temp_result_vec.size() != 0)
		{
			emit hasCompareResult_udp(temp_result_vec, C_GUIDAODIANLU);
			resultVec[5] = temp_result_vec;
		}
	}
	positionVec = Analyze_BaliseMessage::GetPackagePosition("ETCS-27", m_baliseData_Package);
	for (int i = 0; i < positionVec.size(); i++)
	{
		temp_result_vec = Compare_Speed(positionVec[i]);
		if (temp_result_vec.size() != 0)
		{
			emit hasCompareResult_udp(temp_result_vec, C_XIANLUSUDU);
			resultVec[2] = temp_result_vec;
		}
	}
	positionVec = Analyze_BaliseMessage::GetPackagePosition("ETCS-21", m_baliseData_Package);
	for (int i = 0; i < positionVec.size(); i++)
	{
		temp_result_vec = Compare_Grade(positionVec[i]);
		if (temp_result_vec.size() != 0)
		{
			emit hasCompareResult_udp(temp_result_vec, C_XIANLUPODU);
			resultVec[1] = temp_result_vec;
		}
	}
    temp_result_vec = BaliseStation::Compare(m_baliseData_Package,m_Balise_IDstr);
    if (temp_result_vec.size() != 0)
    {
        emit hasCompareResult_udp(temp_result_vec, C_CHEZHAN);
        resultVec[4] = temp_result_vec;
    }
	positionVec = Analyze_BaliseMessage::GetPackagePosition("ETCS-68", m_baliseData_Package);
	for (int i = 0; i < positionVec.size(); i++)
	{
		temp_result_vec = Compare_NeutralSection(positionVec[i]);
		if (temp_result_vec.size() != 0)
		{
			emit hasCompareResult_udp(temp_result_vec, C_FENXIANGQU);
			resultVec[3] = temp_result_vec;
		}
	}
	positionVec = Analyze_BaliseMessage::GetPackagePosition("ETCS-44", m_baliseData_Package);
	for (int i = 0; i < positionVec.size(); i++)
	{
		temp_result_vec = Compare_TrackTemporarySpeedLimit(positionVec[i]);
		if (temp_result_vec.size() != 0)
		{
			emit hasCompareResult_udp(temp_result_vec, C_LINSHIXIANSU);
			resultVec[6] = temp_result_vec;
		}
	}
    resultVec[7] = {{m_Balise_IDstr}};
}

int CompareBaliseData_THREAD::BinarySearchTrackCircuit(QVector<PathWayData>& pathWayData, int baliseLocation, int kmAddFlag)
{
	int low = 0, high = pathWayData.size() - 1;
	while (low <= high) {
		int mid = low + (high - low) / 2;
		int datamid = DesignData::ConvertKmStr2Num(pathWayData[mid].signalSpotKm);
		if (datamid == baliseLocation) {
			return mid;
		}
		else if (datamid > baliseLocation) {
			if (kmAddFlag)
			{
				high = mid - 1;
			}
			else
			{
				low = mid + 1;
			}

		}
		else {
			if (kmAddFlag)
			{
				low = mid + 1;
			}
			else
			{
				high = mid - 1;
			}
		}
	}
	return high;
}

int CompareBaliseData_THREAD::BinarySearchPathWaySpeed(QVector<PathWaySpeed>& pathWaySpeed, int baliseLocation, int kmAddFlag)
{
	int low = 0, high = pathWaySpeed.size() - 1;
	while (low <= high) {
		int mid = low + (high - low) / 2;
		int datamid = DesignData::ConvertKmStr2Num(pathWaySpeed[mid].endKm);
		if (datamid == baliseLocation) {
			return mid;
		}
		else if (datamid > baliseLocation) {
			if (kmAddFlag)
			{
				high = mid - 1;
			}
			else
			{
				low = mid + 1;
			}

		}
		else {
			if (kmAddFlag)
			{
				low = mid + 1;
			}
			else
			{
				high = mid - 1;
			}
		}
	}
	return high;
}

int CompareBaliseData_THREAD::JudjeBrokenLink(QVector<BrokenLink> &brokenLink, int x, int y)
{
	for (int i = 0; i < brokenLink.size(); ++i)
	{
		int brokenLinkKm = DesignData::ConvertKmStr2Num(brokenLink[i].brokenLinkPosSta);
		if (brokenLinkKm >= x && brokenLinkKm <= y)
		{
			int longLen = DesignData::ConvertKmStr2Num(brokenLink[i].brokenLinkLongLen);
			int shortLen = DesignData::ConvertKmStr2Num(brokenLink[i].brokenLinkShortLen);
			return longLen - shortLen;
		}
	}
	return 0;
}


/**********************************************************
 * @功能：比对轨道电路
 * @形参：
 * @返回值： 应答器编号 + 原始数据索引
 *          起点（应答器/表）比对结果
 *          信号机类型 （应答器/表）比对结果
 *          载频（应答器/表）比对结果
 *          长度（应答器/表）比对结果
 * @备注：
 * @作者：hb
 * @修改日期：
**********************************************************/
QVector<QVector<QString> > CompareBaliseData_THREAD::Compare_TrackCircuit(int position)//比对轨道电路
{
	QVector<QVector<QString> > compareResultVec;
	if ((DesignData::pathWayDataUpProVec.empty() && g_LineDirection == UP && g_TrainDirection == FRONT) || (DesignData::pathWayDataUpBackVec.empty() && g_LineDirection == UP && g_TrainDirection == BACK) || (DesignData::pathWayDataDownProVec.empty() && g_LineDirection == DOWN && g_TrainDirection == FRONT) || (DesignData::pathWayDataDownBackVec.empty() && g_LineDirection == DOWN && g_TrainDirection == BACK))//如果没有导入相应的列控数据，那么就退出
		return compareResultVec;
	bool permission = false; //用来判断是否进行后续对比
	QVector<Item> ETCS_44;
	ETCS_44 = m_baliseData_Package[position];
	QVector<Item> NID_XUSER_Vec = Analyze_BaliseMessage::FindItem("NID_XUSER", ETCS_44);
	if (NID_XUSER_Vec[1].value != 1) //没有CTCS1包
	{
		return compareResultVec;
	}
	//有CTCS1包，继续判断数据包验证方向与列车运行方向是否符合
	QVector<Item> Q_DIR_Vec = Analyze_BaliseMessage::FindItem("Q_DIR", ETCS_44);
	if ((!Q_DIR_Vec[1].value && g_TrainDirection == BACK) || (Q_DIR_Vec[1].value == 1 && g_TrainDirection == FRONT)
		|| Q_DIR_Vec[1].value == 2) //如果列车运行方向与信息包验证方向符合才进行后续对比
	{
		permission = true;
	}
	if (permission)
	{
		static int s_baliseLen2Signal;//存储上一个应答器组中应答器据前方第一个信号机的距离，用于确定侧线应答器相对位置
		QVector<QString> item(4);
		item[0] = m_Balise_IDstr;
		item[1] = QString::number(m_udpDataOrder, 10);
		if (m_originBaliseBitMessage.type &BaliseType::SideLine)
		{
			item[2] = "(侧线)";
		}
		else
		{
			item[3] = "";
		}
		compareResultVec.push_back(item);
		//填充应答器信息
		QVector<Item> NID_SIGNALVec_balise = Analyze_BaliseMessage::FindItem("NID_SIGNAL", ETCS_44);
		QVector<Item> NID_FREQUENCYVec_balise = Analyze_BaliseMessage::FindItem("NID_FREQUENCY", ETCS_44);
		QVector<Item> L_SECTION_balise = Analyze_BaliseMessage::FindItem("L_SECTION", ETCS_44);
		QVector<Item> N_ITER_balise = Analyze_BaliseMessage::FindItem("N_ITER", ETCS_44);
		int D_SIGNAL_balise = Analyze_BaliseMessage::FindItem("D_SIGNAL", ETCS_44)[1].value;

		QVector<QString> signalTypeVec_balise; //存储应答器中描述的信号机类型
		QVector<QString> freVec_balise;        //存储应答器中描述的载频类型
		QVector<QString> lengthVec_balise;      //存储应答器中描述的轨道区段长度

		int iterNum_balise = N_ITER_balise.isEmpty() ? 0 : N_ITER_balise[1].value;

		for (int i = 1; i < NID_SIGNALVec_balise.size(); ++i)
		{
			QString str;
			switch (NID_SIGNALVec_balise[i].value) {
			case 0: str = "没有信号机";                break;
			case 1: str = "进站信号机";                break;
			case 2: str = "出站信号机";                break;
			case 3: str = "通过信号机";                break;
			case 4: str = "进路信号机";                break;
			case 5: str = "调车信号机";                break;
			case 6: str = "出站口";                    break;
			case 7: str = "出站信号机";                break;
			default:                                   break;
			}
			signalTypeVec_balise.append(str);
		}
		for (int i = 1; i < NID_FREQUENCYVec_balise.size(); ++i)
		{
			QString str;
			switch (NID_FREQUENCYVec_balise[i].value)
			{
			case 0: str = "0";         break;
			case 1: str = "1700";      break;
			case 2: str = "2000";      break;
			case 3: str = "2300";      break;
			case 4: str = "2600";      break;
			case 5: str = "1700-1";    break;
			case 6: str = "1700-2";    break;
			case 7: str = "2000-1";    break;
			case 8: str = "2000-2";    break;
			case 9: str = "2300-1";    break;
			case 10: str = "2300-2";   break;
			case 11: str = "2600-1";   break;
			case 12: str = "2600-2";   break;
			default:                   break;
			}
			freVec_balise.append(str);
		}
		for (int i = 1; i < L_SECTION_balise.size(); ++i)
		{
			lengthVec_balise.append(QString::number(L_SECTION_balise[i].value, 10));
		}
		if (SideLineIndex == -1)
		{
			item[0] = "正线应答器";
		}
		else if (SideLineIndex == 0)
		{
			item[0] = "进站应答器";
		}
		else if (SideLineIndex == 1)
		{
			item[0] = "反出站应答器";
		}
		else if (SideLineIndex == 2)
		{
			item[0] = "出站应答器";
		}
		else
		{
			item[0] = "反进站应答器";
		}
		item[1] = "";
		item[2] = "";
		item[3] = "";
		compareResultVec.push_back(item);

		//建立表头
		item[0] = "应答器描述项";
		item[1] = "应答器内容";
		item[2] = "数据表内容";
		item[3] = "比对结果";
		compareResultVec.push_back(item);

		//接下来需要针对不同应答器类型生成要比对的工程数据表
		QVector<PathWayData_Less> pathWayData_LessVec;
		if (SideLineIndex > 3 || SideLineIndex == JZ || SideLineIndex == FJZ) //-1为正线应答器，其全部内容为正线数据，0为进站应答器，其部分内容为正线数据，部分为侧线数据，3为出站应答器，其全部内容为正线数据
		{
			bool kmAddFlag = false;
			QVector<PathWayData> pathWayDataVec_temp;
			if (g_LineDirection == UP)//选用线路数据表
			{
				if (g_TrainDirection == FRONT)
				{
					kmAddFlag = false;
					pathWayDataVec_temp = DesignData::pathWayDataUpProVec;
				}
				else
				{
					kmAddFlag = true;
					pathWayDataVec_temp = DesignData::pathWayDataUpBackVec;
				}
			}
			else
			{
				if (g_TrainDirection == FRONT)
				{
					kmAddFlag = true;
					pathWayDataVec_temp = DesignData::pathWayDataDownProVec;
				}
				else
				{
					kmAddFlag = false;
					pathWayDataVec_temp = DesignData::pathWayDataDownBackVec;
				}
			}

			//在线路数据表中确定应答器后方第一架信号机的索引
			int p = BinarySearchTrackCircuit(pathWayDataVec_temp, m_baliseLocation, kmAddFlag);
			if (p == pathWayDataVec_temp.size() - 1 || m_baliseLocation == -1) { //这时应答器已经超出线路数据表中最后一个应答器位置，无法由后一个信号点位置确定应答器到信号点距离 或者 没有匹配到任何应答器
				//第一项D_SIGNAL
				item[0] = "D_SIGNAL";
				item[1] = QString::number(D_SIGNAL_balise);
				item[2] = "无匹配";
				item[3] = "不一致";
				compareResultVec.push_back(item);
			}
			else { //前方至少存在一个信号机
				//计算应答器到其运行方向前方第一架信号机的距离，由于是公里标相加减，需要考虑两公里标之间有无断链
				int nextPositionKm = DesignData::ConvertKmStr2Num(pathWayDataVec_temp[p + 1].signalSpotKm);
				int brokenLinkLen = JudjeBrokenLink(DesignData::brokenLinkVec, m_baliseLocation, nextPositionKm);
				int baliseLen2Signal = abs(nextPositionKm - m_baliseLocation) + brokenLinkLen;
				s_baliseLen2Signal = baliseLen2Signal;
				//先比D_SIGNAL
				if (D_SIGNAL_balise == 0)
				{
					//第一项D_SIGNAL
					item[0] = "D_SIGNAL";
					item[1] = QString::number(0);
					item[2] = item[1];
					item[3] = "一致";
					compareResultVec.push_back(item);

					//需要将 baliseLen2Signal 加入生成的比对的工程数据表，因为应答器描述中第一个字段会描述baliseLen2Signal这段信息
					QString pathWayFre_temp = p == -1 ? "无描述" : pathWayDataVec_temp[p].pathWayFre;
					QString signalSpotType_temp = p == pathWayDataVec_temp.size() - 1 ? "无描述" : pathWayDataVec_temp[p + 1].signalSpotType;
					PathWayData_Less pl(signalSpotType_temp, pathWayFre_temp, baliseLen2Signal);
					pathWayData_LessVec.push_back(pl);
				}
				else
				{
					item[0] = "D_SIGNAL";
					item[1] = QString::number(D_SIGNAL_balise);
					item[2] = QString::number(baliseLen2Signal);
					item[3] = item[1] == item[2] ? "一致" : "不一致";
					compareResultVec.push_back(item);
				}
				//开始补充后续内容
				if (SideLineIndex == JZ) //进站应答器，后续内容为进路数据表中描述的轨道区段信息
				{
					for (int i = 0; i < SideLineInfoIn.pathWay_Length.size(); i++)
					{
						PathWayData_Less pl(SideLineInfoIn.pathWay_SignalType[i], SideLineInfoIn.pathWay_Fre[i], SideLineInfoIn.pathWay_Length[i]);
						pathWayData_LessVec.push_back(pl);
					}
					if (!SideLineInfoOut.isEmpty()) //站内只有一个应答器的进路。
					{
						//继续添加出站信号机到出站口的轨道电路
						for (int i = 0; i < SideLineInfoOut.pathWay_Length.size(); i++)
						{
							PathWayData_Less pl(SideLineInfoOut.pathWay_SignalType[i], SideLineInfoOut.pathWay_Fre[i], SideLineInfoOut.pathWay_Length[i]);
							pathWayData_LessVec.push_back(pl);
						}

						//继续添加出站口之后的轨道电路
						//根据E5包的链接应答器和列车通过被应答器的方向来决定使用那条线路数据
						if (!ALGI.isEmpty())
						{
							int stationID = ALGI.back().ID.split("-").back().toInt();
							if (stationID % 2 == 0)
							{
								kmAddFlag = ALGI.back().Dir == FRONT ? false : true;
								pathWayDataVec_temp = ALGI.back().Dir == FRONT ? DesignData::pathWayDataUpProVec : DesignData::pathWayDataUpBackVec;
							}
							else
							{
								kmAddFlag = ALGI.back().Dir == FRONT ? true : false;
								pathWayDataVec_temp = ALGI.back().Dir == FRONT ? DesignData::pathWayDataDownProVec : DesignData::pathWayDataDownBackVec;
							}
						}
						//在线路数据表中确定反进站站应答器后方第一架信号机的索引
						int baliseLocation_FJZ = DesignData::FindBaliseKmByID(QString("%1-1").arg(ALGI.back().ID));
						if (baliseLocation_FJZ != -1)
						{
							int position = BinarySearchTrackCircuit(pathWayDataVec_temp, baliseLocation_FJZ, kmAddFlag);
							//开始获取数据
							if (position != -1)
							{
								for (int i = position; i + 1 < pathWayDataVec_temp.size(); i++)
								{
									PathWayData_Less pl(pathWayDataVec_temp[i + 1].signalSpotType, pathWayDataVec_temp[i].pathWayFre, pathWayDataVec_temp[i].pathWayLen.toInt());
									pathWayData_LessVec.push_back(pl);
								}
							}
						}
					}
				}
				else                   //正线和出站应答器，后续内容为线路数据表中正线内容
				{
					for (int i = p + 1; i + 1 < pathWayDataVec_temp.size(); i++)
					{
						PathWayData_Less pl(pathWayDataVec_temp[i + 1].signalSpotType, pathWayDataVec_temp[i].pathWayFre, pathWayDataVec_temp[i].pathWayLen.toInt());
						pathWayData_LessVec.push_back(pl);
					}
				}
			}
		}
		if (SideLineIndex == FCZ) //如果是反出站应答器
		{
			bool kmAddFlag = false;
			QVector<PathWayData> pathWayDataVec_temp;
			//定位应答器所在轨道电路
			if (SideLineInfoIn.balise_Diatance.size() >= 2)
			{
				int distance = SideLineInfoIn.balise_Diatance[0]; //反出站应答器与进站应答器的链接距离
				int baliseDistance2Signal = distance - s_baliseLen2Signal; //反出站应答器距进站信号机的相对距离
				//开始定标反出站应答器
				int sectionStart = 0;
				int sumLen = 0;
				for (int i = 0; i < SideLineInfoIn.pathWay_Length.size(); i++)
				{
					sumLen += SideLineInfoIn.pathWay_Length[i];
					if (sumLen >= baliseDistance2Signal)
					{
						sectionStart = i;
						break;
					}
				}
				//计算应答器到其运行方向前方第一架信号机的距离
				int baliseLen2Signal = sumLen - baliseDistance2Signal;

				//先比D_SIGNAL
				if (D_SIGNAL_balise == 0)
				{
					//第一项D_SIGNAL
					item[0] = "D_SIGNAL";
					item[1] = QString::number(0);
					item[2] = item[1];
					item[3] = "一致";
					compareResultVec.push_back(item);

					//需要将应答器器据其运行前方第一架信号机的距离等存入待生成的工程数据表
					PathWayData_Less pl(SideLineInfoIn.pathWay_SignalType[sectionStart], SideLineInfoIn.pathWay_Fre[sectionStart], baliseLen2Signal);
					pathWayData_LessVec.push_back(pl);
				}
				else
				{
					//第一项D_SIGNAL
					item[0] = "D_SIGNAL";
					item[1] = QString::number(D_SIGNAL_balise);
					item[2] = QString::number(baliseLen2Signal);
					item[3] = item[1] == item[2] ? "一致" : "不一致";
					compareResultVec.push_back(item);
				}

				//开始补充后续内容, 分为两种情况：1.反出站应答器中只描述到出站应答器，出站应答器中写有停车报文
				//                                2.该应答器中为侧线通过的预告报文,即不仅描述了到出站应答器的轨道电路，还描述了出站应答器到即将行走的线路的轨道电路信息
				for (int i = sectionStart + 1; i < SideLineInfoIn.pathWay_Length.size(); i++) //从反出站应答器到出站应答器的部分
				{
					PathWayData_Less pl(SideLineInfoIn.pathWay_SignalType[i], SideLineInfoIn.pathWay_Fre[i], SideLineInfoIn.pathWay_Length[i]);
					pathWayData_LessVec.push_back(pl);
				}
				if (!SideLineInfoOut.isEmpty()) //对应第二种情况，填充区段为从出站应答器到反进站应答器 和 反进站应答器所在线路部分
				{
					for (int i = 0; i < SideLineInfoOut.pathWay_Length.size(); i++)
					{
						PathWayData_Less pl(SideLineInfoOut.pathWay_SignalType[i], SideLineInfoOut.pathWay_Fre[i], SideLineInfoOut.pathWay_Length[i]);
						pathWayData_LessVec.push_back(pl);
					}
					//根据E5包的链接应答器和列车通过被应答器的方向来决定使用那条线路数据
					if (!ALGI.isEmpty())
					{
						int stationID = ALGI.back().ID.split("-").back().toInt();
						if (stationID % 2 == 0)
						{
							kmAddFlag = ALGI.back().Dir == FRONT ? false : true;
							pathWayDataVec_temp = ALGI.back().Dir == FRONT ? DesignData::pathWayDataUpProVec : DesignData::pathWayDataUpBackVec;
						}
						else
						{
							kmAddFlag = ALGI.back().Dir == FRONT ? true : false;
							pathWayDataVec_temp = ALGI.back().Dir == FRONT ? DesignData::pathWayDataDownProVec : DesignData::pathWayDataDownBackVec;
						}
					}
					//在线路数据表中确定反进站站应答器后方第一架信号机的索引
					int baliseLocation_FJZ = DesignData::FindBaliseKmByID(QString("%1-1").arg(ALGI.back().ID));
					if (baliseLocation_FJZ != -1)
					{
						int position = BinarySearchTrackCircuit(pathWayDataVec_temp, baliseLocation_FJZ, kmAddFlag);
						//开始获取数据
						if (position != -1)
						{
							for (int i = position; i + 1 < pathWayDataVec_temp.size(); i++)
							{
								PathWayData_Less pl(pathWayDataVec_temp[i + 1].signalSpotType, pathWayDataVec_temp[i].pathWayFre, pathWayDataVec_temp[i].pathWayLen.toInt());
								pathWayData_LessVec.push_back(pl);
							}
						}
					}
				}
			}
		}
		if (SideLineIndex == CZ)//出站信号机
		{
			bool kmAddFlag = false;
			QVector<PathWayData> pathWayDataVec_temp;
			//出站信号机中有轨道电路描述，则必为侧向通过或侧向发车。需填充出站应答器到反进站应答器及反进站应答器之后这两段轨道电路信息
			int sectionStart = 0;
			int baliseLen2Signal = -1;
            //bug point
			QString baliseRemark = DesignData::FindBaliseRemarkByID(m_Balise_IDstr);
			if (baliseRemark != "")
			{
				QString temp = "";
				for (int i = 0; i < baliseRemark.size(); i++)
				{
					if (baliseRemark[i] >= '0' && baliseRemark[i] <= '9')
					{
						temp += baliseRemark[i];
					}
				}
				baliseLen2Signal = temp.toInt();
			}
			if (SideLineInfoIn.balise_Diatance.size() >= 2)//这是侧向通过
			{
				int distance = SideLineInfoIn.balise_Diatance[0] + SideLineInfoIn.balise_Diatance[1]; //出站应答器与进站应答器的链接距离
				int baliseDistance2Signal = distance - s_baliseLen2Signal; //出站应答器距进站信号机的相对距离
																		   //开始定标反出站应答器
				int sumLen = 0;
				for (int i = 0; i < SideLineInfoIn.pathWay_Length.size(); i++)
				{
					sumLen += SideLineInfoIn.pathWay_Length[i];
					if (sumLen >= baliseDistance2Signal)
					{
						sectionStart = i;
						break;
					}
				}
				//计算应答器到其运行方向前方第一架信号机的距离
				baliseLen2Signal = sumLen - baliseDistance2Signal;
			}
			//先比D_SIGNAL
			if (D_SIGNAL_balise == 0)
			{
				//第一项D_SIGNAL
				item[0] = "D_SIGNAL";
				item[1] = QString::number(0);
				item[2] = item[1];
				item[3] = "一致";
				compareResultVec.push_back(item);

				//需要将应答器器据其运行前方第一架信号机的距离等存入待生成的工程数据表
				if (SideLineInfoIn.balise_Diatance.size() >= 2)
				{
					PathWayData_Less pl(SideLineInfoIn.pathWay_SignalType[sectionStart], SideLineInfoIn.pathWay_Fre[sectionStart], baliseLen2Signal);
					pathWayData_LessVec.push_back(pl);
				}
				else
				{
					PathWayData_Less pl("无法获得", "无法获得", baliseLen2Signal);
					pathWayData_LessVec.push_back(pl);
				}

			}
			else
			{
				//第一项D_SIGNAL
				item[0] = "D_SIGNAL";
				item[1] = QString::number(D_SIGNAL_balise);
				if (baliseLen2Signal == -1)
				{
					item[2] = "无法获得";
					item[3] = "";
				}
				else
				{
					item[2] = QString::number(baliseLen2Signal);
					item[3] = item[2] == item[1] ? "一致" : "不一致";
				}
				compareResultVec.push_back(item);
			}
			//开始填充出站信号机到反进站应答器及反进站应答器之后部分
			if (!SideLineInfoOut.isEmpty())
			{
				for (int i = 0; i < SideLineInfoOut.pathWay_Length.size(); i++)
				{
					PathWayData_Less pl(SideLineInfoOut.pathWay_SignalType[i], SideLineInfoOut.pathWay_Fre[i], SideLineInfoOut.pathWay_Length[i]);
					pathWayData_LessVec.push_back(pl);
				}
				//根据E5包的链接应答器和列车通过被应答器的方向来决定使用那条线路数据
				if (!ALGI.isEmpty())
				{
					int stationID = ALGI.front().ID.split("-").back().toInt();
					if (stationID % 2 == 0)
					{
						kmAddFlag = ALGI.back().Dir == FRONT ? false : true;
						pathWayDataVec_temp = ALGI.back().Dir == FRONT ? DesignData::pathWayDataUpProVec : DesignData::pathWayDataUpBackVec;
					}
					else
					{
						kmAddFlag = ALGI.back().Dir == FRONT ? true : false;
						pathWayDataVec_temp = ALGI.back().Dir == FRONT ? DesignData::pathWayDataDownProVec : DesignData::pathWayDataDownBackVec;
					}
				}
				//在线路数据表中确定应答器后方第一架信号机的索引
				int baliseLocation_FJZ = DesignData::FindBaliseKmByID(QString("%1-1").arg(ALGI.front().ID));
				if (baliseLocation_FJZ != -1)
				{
					int position = BinarySearchTrackCircuit(pathWayDataVec_temp, baliseLocation_FJZ, kmAddFlag);
					//开始获取数据
					if (position != -1)
					{
						for (int i = position; i + 1 < pathWayDataVec_temp.size(); i++)
						{
							PathWayData_Less pl(pathWayDataVec_temp[i + 1].signalSpotType, pathWayDataVec_temp[i].pathWayFre, pathWayDataVec_temp[i].pathWayLen.toInt());
							pathWayData_LessVec.push_back(pl);
						}
					}
				}
			}
		}

		//开始比对
		for (int i = 0, p = 0; i < iterNum_balise + 1; i++, p++)
		{
			if (p >= pathWayData_LessVec.size()) //已经匹配到工程数据表最后一个位置了，此时信号机类型无法确定
			{
				//第一项信号机类型
				item[0] = "信号机类型";
				item[1] = signalTypeVec_balise[i];
				item[2] = "无匹配";
				item[3] = "不一致";
				compareResultVec.push_back(item);

				//第二项载频
				item[0] = "载频";
				item[1] = freVec_balise[i];
				item[2] = "无匹配";
				item[3] = "不一致";
				compareResultVec.push_back(item);

				//第三项长度
				item[0] = "长度";
				item[1] = lengthVec_balise[i];
				item[2] = "无匹配";;
				item[3] = "不一致";
				compareResultVec.push_back(item);
			}
			else
			{
				//首先比对长度，因为可能存在合并
				if (lengthVec_balise[i].toInt() <= pathWayData_LessVec[p].pathWayLen) //不可能有合并
				{
					//第一项信号机类型
					item[0] = "信号机类型";
					item[1] = signalTypeVec_balise[i];
					item[2] = pathWayData_LessVec[p].signalSpotType;
					item[3] = item[2] == item[1] ? "一致" : "不一致";
					compareResultVec.push_back(item);

					//第二项载频
					item[0] = "载频";
					item[1] = freVec_balise[i];
					item[2] = pathWayData_LessVec[p].pathWayFre.split("-").front();
					item[3] = item[2] == item[1] ? "一致" : "不一致";
					compareResultVec.push_back(item);

					//第三项长度
					item[0] = "长度";
					item[1] = lengthVec_balise[i];
					if (pathWayData_LessVec[p].pathWayLen == -1)
					{
						item[2] = "无法获得";
						item[3] = "";
					}
					else
					{
						item[2] = QString::number(pathWayData_LessVec[p].pathWayLen);
						item[3] = item[2] == item[1] ? "一致" : "不一致";
					}
					compareResultVec.push_back(item);
				}
				else   //可能存在合并
				{
					int length_combine = -1;
					QString signalType_combine = pathWayData_LessVec[i].signalSpotType;
					QString fre_combine = pathWayData_LessVec[i].pathWayFre;
					judjeTrackCircuitCombine(pathWayData_LessVec, p, lengthVec_balise[i].toInt(), signalTypeVec_balise[i], freVec_balise[i], length_combine, signalType_combine, fre_combine);

					if (length_combine == -1) //不存在合并
					{
						//第一项信号机类型
						item[0] = "信号机类型";
						item[1] = signalTypeVec_balise[i];
						item[2] = pathWayData_LessVec[p].signalSpotType;
						item[3] = item[2] == item[1] ? "一致" : "不一致";
						compareResultVec.push_back(item);

						//第二项载频
						item[0] = "载频";
						item[1] = freVec_balise[i];
						item[2] = pathWayData_LessVec[p].pathWayFre.split("-").front();
						item[3] = item[2] == item[1] ? "一致" : "不一致";
						compareResultVec.push_back(item);

						//第三项长度
						item[0] = "长度";
						item[1] = lengthVec_balise[i];
						if (pathWayData_LessVec[p].pathWayLen == -1)
						{
							item[2] = "无法获得";
							item[3] = "";
						}
						else
						{
							item[2] = QString::number(pathWayData_LessVec[p].pathWayLen);
							item[3] = item[2] == item[1] ? "一致" : "不一致";
						}
						compareResultVec.push_back(item);
					}
					else //存在合并
					{
						//第一项信号机类型
						item[0] = "信号机类型";
						item[1] = signalTypeVec_balise[i];
						item[2] = signalType_combine;
						item[3] = item[2] == item[1] ? "一致" : "不一致";
						compareResultVec.push_back(item);

						//第二项载频
						item[0] = "载频";
						item[1] = freVec_balise[i];
						item[2] = fre_combine.split("-").front();
						item[3] = item[2] == item[1] ? "一致" : "不一致";
						compareResultVec.push_back(item);

						//第三项长度
						item[0] = "合并长度";
						item[1] = lengthVec_balise[i];
						item[2] = QString::number(length_combine);
						item[3] = item[2] == item[1] ? "一致" : "不一致";
						compareResultVec.push_back(item);
					}
				}

			}
		}
		return compareResultVec;
	}
	else
	{
		return compareResultVec;
	}
}




/**********************************************************
 * @功能：判断两个字符串容器对应项是否相等
 * @形参：两个字符串容器
 * @返回值：QVector<QString>类型的引用， 大小不一致返回空容器
 * @备注：
 * @作者：hb
 * @修改日期：
**********************************************************/
QVector<QString> CompareBaliseData_THREAD::CompareStringVec(QVector<QString>& str1, QVector<QString>& str2)
{
	QVector<QString> result;
	int minSize = 0;
	QString forCom1;
	QString forCom2;
	if (str1.size() == str2.size())
	{
		for (int i = 0; i < str1.size(); i++)
		{
			minSize = str1[i].size() > str2[i].size() ? str2[i].size() : str1[i].size();
			forCom1 = str1[i].mid(0, minSize);
			forCom2 = str2[i].mid(0, minSize);

			if (forCom1.contains("无描述") || forCom2.contains("无描述")
				|| forCom1 == "" || forCom2 == "")
			{
				result.append("");
			}
			else if (forCom1 == forCom2)
			{
				result.append("一致");
			}
			else
			{
				result.append("不一致");
			}
		}
	}
	return result;
}


QVector<QString> CompareBaliseData_THREAD::CompareGradeVec(QVector<QString>& str1, QVector<QString>& str2)
{
	QVector<QString> result;
	for (int i = 0; i < str1.size(); i++)
	{
		if (str1[i].contains("无描述") || str2[i].contains("无描述")
			|| str1[i] == "" || str2[i] == "")
		{
			result.append("");
			continue;
		}
		else
		{
			bool ok = false;
			int num_1 = str1[i].toInt(&ok);
			double num_2 = str2[i].toDouble(&ok);
			if (ok)
			{
				int num_2_int;
				num_2_int = floor(num_2);
				if (num_1 == num_2_int)
					result.append("一致");
				else
					result.append("不一致");
			}
			else
				result.append("double参数转换int出现问题");
		}
	}
	return result;
}

/**********************************************************
* @功能：判断公里标是否相等，并给出误差
* @形参：两个字符串容器,第一个是应答器公里标， 第二个是表中公里标
* @返回值：QVector<QString>
* @备注：
* @作者：hb
* @修改日期：
**********************************************************/
QVector<QString> CompareBaliseData_THREAD::CompareStringVec_Km(QVector<QString>& str_baliseVec, QVector<QString>& str_sheetVec)
{
	int km_balise = 0;
	int km_sheet = 0;
	int error = 0;
	QVector<QString> resultVec;
	QStringList strListTemp;
	if (str_baliseVec.size() == str_sheetVec.size())
	{
		for (int i = 0; i < str_baliseVec.size(); i++)
		{
			km_sheet = DesignData::ConvertKmStr2Num(str_sheetVec[i]);
			if (str_baliseVec[i].contains("-")) //这里是为了处理长短链的增减信息
			{
				if (str_baliseVec[i].startsWith('-'))
				{
					km_balise = DesignData::ConvertKmStr2Num(str_baliseVec[i]);
				}
				else
				{
					strListTemp = str_baliseVec[i].split("-");
					km_balise = DesignData::ConvertKmStr2Num(strListTemp[0]);
					for (int i = 1; i < strListTemp.size(); i++)
					{
						km_balise -= convert2Km(strListTemp[i]);
					}
				}
			}
			else if (str_baliseVec[i].contains("+"))
			{
				strListTemp = str_baliseVec[i].split("+");
				km_balise = DesignData::ConvertKmStr2Num(strListTemp[0]);
				for (int i = 1; i < strListTemp.size(); i++)
				{
					km_balise += convert2Km(strListTemp[i]);
				}
			}
			else
			{
				km_balise = DesignData::ConvertKmStr2Num(str_baliseVec[i]);
			}

			if (str_baliseVec[i].contains("无描述") || str_sheetVec[i].contains("无描述") ||
				str_baliseVec[i] == "" || str_sheetVec[i] == "")
			{
				resultVec.append("");
			}
			else if (km_balise == km_sheet)
			{
				resultVec.append("一致         0");
			}
			else
			{
				if (km_sheet != 0)
				{
					error = abs(km_balise - km_sheet);
					resultVec.append("不一致   " + QString::number(error, 10));
				}
				else
				{
					resultVec.append("不一致");
				}

			}
		}
	}
	return resultVec;
}

QVector <QString> CompareBaliseData_THREAD::CompareSpeedLenVec(QVector<QString>& str_baliseVec, QVector<QString>& str_sheetVec)
{
	int error = 0;
	int km_balise = 0;
	QVector<QString> resultVec;
	int km_sheet = 0;
	for (int i = 0; i < str_baliseVec.size(); i++)
	{
		km_sheet = DesignData::ConvertKmStr2Num(str_sheetVec[i]);
		km_balise = DesignData::ConvertKmStr2Num(str_baliseVec[i]);
		if (str_baliseVec[i].contains("无描述") || str_sheetVec[i].contains("无描述") ||
			str_baliseVec[i] == "" || str_sheetVec[i] == "")
		{
			resultVec.append("");
		}
		else if (km_balise < km_sheet)
		{
			resultVec.append("在终点范围内");
		}
		else if (km_balise == km_sheet)
		{
			resultVec.append("一致");
		}
		else
		{
			error = abs(km_balise - km_sheet);
			resultVec.append("不一致   " + QString::number(error, 10));
		}
	}
	return resultVec;
}


QVector<QString> CompareBaliseData_THREAD::CompareSpeedVec(QVector<QString>& str_baliseVec, QVector<QString>& str_sheetVec, bool isAddKm)
{
	int km_balise = 0;
	int km_sheet = 0;
	int error = 0;
	QVector<QString> resultVec;
	if (str_baliseVec.size() == str_sheetVec.size())
	{
		for (int i = 0; i < str_baliseVec.size(); i++)
		{
			//处理应答器终点公里标中可能存在的长短链信息
			QStringList strListTemp;
			if (str_baliseVec[i].contains("-"))
			{
				strListTemp = str_baliseVec[i].split("-");
				if (strListTemp.size() == 2)
				{
					km_balise = DesignData::ConvertKmStr2Num(strListTemp[0]);
					km_balise = km_balise - convert2Km(strListTemp[1]);
				}
			}
			else if (str_baliseVec[i].contains("+"))
			{
				strListTemp = str_baliseVec[0].split("+");
				if (strListTemp.size() == 2)
				{
					km_balise = DesignData::ConvertKmStr2Num(strListTemp[0]);
					km_balise = km_balise + convert2Km(strListTemp[1]);
				}
			}
			else
			{
				km_balise = DesignData::ConvertKmStr2Num(str_baliseVec[i]);
			}
			km_sheet = DesignData::ConvertKmStr2Num(str_sheetVec[i]);

			if (str_baliseVec[i].contains("无描述") || str_sheetVec[i].contains("无描述") ||
				str_baliseVec[i] == "" || str_sheetVec[i] == "")
			{
				resultVec.append("");
			}
			else if (km_balise < km_sheet)
			{
				if (isAddKm == true)
				{
					resultVec.append("在终点范围内");
				}
				else
				{
					error = abs(km_balise - km_sheet);
					resultVec.append("不一致   " + QString::number(error, 10));
				}
			}
			else if (km_balise > km_sheet)
			{
				if (isAddKm == true)
				{
					error = abs(km_balise - km_sheet);
					resultVec.append("不一致   " + QString::number(error, 10));
				}
				else
				{
					resultVec.append("在终点范围内");
				}
			}
			else
			{
				resultVec.append("一致");
			}
		}
	}
	return resultVec;
}

int CompareBaliseData_THREAD::convert2Km(const QString& str)
{
	QString strTemp;
	for (int i = 0; i < str.size(); i++)
	{
		if (str[i] >= '0' && str[i] <= '9')
		{
			strTemp.append(str[i]);
		}
	}
	return strTemp.toInt();
}

/*流程图

		使用FindItem获取分相区长度和距离容器
					|
					v
		根据应答器编号查找应答器位置表得到对应公里标
					|
					v
		分相区距离 + 应答器位置 = 起点位置（应答器）
		起点位置（应答器） + 长度 = 终点位置（应答器）
					|
					v
		for循环嵌套，将每一个起点与分相区信息表中起点对比，若匹配，则将分相区信息表对应项及以后(以前)固定个数的起点位置和终点位置返回
					|
					v
		将起点位置（应答器）、起点位置（表）、终点位置（应答器）、终点位置（表）作为消息参数发送
*/


QVector<QVector<QString> > CompareBaliseData_THREAD::Compare_NeutralSection(int position)
{
	QVector<QVector<QString> > compareResultVec;
	if ((DesignData::neutralSectionUpVec.empty() && g_LineDirection == UP) || (DesignData::neutralSectionDownVec.empty() && g_LineDirection == DOWN))//如果没有导入相应的列控数据，那么就退出
		return compareResultVec;
	bool permission = false; //用来判断是否进行后续对比
	QVector<Item> ETCS_68;
	ETCS_68 = m_baliseData_Package[position];
	//判断数据包验证方向与列车运行方向是否符合
	QVector<Item> Q_DIR_Vec = Analyze_BaliseMessage::FindItem("Q_DIR", ETCS_68);
	if ((Q_DIR_Vec[1].value == 0 && g_TrainDirection == BACK) || (Q_DIR_Vec[1].value == 1 && g_TrainDirection == FRONT)
		|| Q_DIR_Vec[1].value == 2) //如果列车运行方向与信息包验证方向符合才进行后续对比
	{
		permission = true;
	}

	if (permission)
	{
		QVector<QString> item(4);
		item[0] = m_Balise_IDstr;
		item[1] = QString::number(m_udpDataOrder, 10);
		if (m_originBaliseBitMessage.type &BaliseType::SideLine)
		{
			item[2] = "(侧线)";
		}
		else
		{
			item[3] = "";
		}
		compareResultVec.push_back(item);

		if (SideLineIndex == -1)
		{
			item[0] = "正线应答器";
		}
		else if (SideLineIndex == 0)
		{
			item[0] = "进站应答器";
		}
		else if (SideLineIndex == 1)
		{
			item[0] = "反出站应答器";
		}
		else if (SideLineIndex == 2)
		{
			item[0] = "出站应答器";
		}
		else
		{
			item[0] = "反进站应答器";
		}
		item[1] = "";
		item[2] = "";
		item[3] = "";
		compareResultVec.push_back(item);

		//使用FindItem获取分相区长度和距离容器
		QVector<Item> D_TRACKCONDVec_balise = Analyze_BaliseMessage::FindItem("D_TRACKCOND", ETCS_68);
		QVector<Item> L_TRACKCONDVec_balise = Analyze_BaliseMessage::FindItem("L_TRACKCOND", ETCS_68);

		QVector<int> distance_balise;
		QVector<int> length_balise;
		for (int i = 1; i < D_TRACKCONDVec_balise.size(); i++)
		{
			distance_balise.append(D_TRACKCONDVec_balise[i].value);
		}
		for (int i = 1; i < L_TRACKCONDVec_balise.size(); i++)
		{
			length_balise.append(L_TRACKCONDVec_balise[i].value);
		}

		//根据上下行选定工程数据表
		QVector<NeutralSection> neutralSectionVec_temp = g_LineDirection == UP ? DesignData::neutralSectionUpVec : DesignData::neutralSectionDownVec;

		//判断公里标增减
		bool kmAddFlag = false;
		if ((g_LineDirection == DOWN && g_TrainDirection == FRONT) ||
			(g_LineDirection == UP && g_TrainDirection == BACK)) //下行正向或者上行反向公里标增大
		{
			kmAddFlag = true;
		}
		else
		{
			kmAddFlag = false;
		}


		//获取数据表中分相区起点位置
		QVector<int> sectionStartKmVec_sheet;
		QVector<int> sectionLenVec_sheet;

		if (g_TrainDirection == FRONT)
		{
			for (int i = 0; i < neutralSectionVec_temp.size(); i++)
			{
				sectionStartKmVec_sheet.append(DesignData::ConvertKmStr2Num(neutralSectionVec_temp[i].beganKm));
				sectionLenVec_sheet.append(neutralSectionVec_temp[i].len.toInt());
			}
		}
		else
		{
			for (int i = neutralSectionVec_temp.size() - 1; i >= 0; i--)
			{
				sectionStartKmVec_sheet.append(DesignData::ConvertKmStr2Num(neutralSectionVec_temp[i].endKm));
				sectionLenVec_sheet.append(neutralSectionVec_temp[i].len.toInt());
			}
		}

		//找到应答器运行前方第一个分相区起始点位置
		int p;
		for (int i = 0; i < sectionStartKmVec_sheet.size(); i++)
		{
			if (kmAddFlag)
			{
				if (sectionStartKmVec_sheet[i] >= m_baliseLocation)
				{
					p = i;
					break;
				}
			}
			else
			{
				if (sectionStartKmVec_sheet[i] <= m_baliseLocation)
				{
					p = i;
					break;
				}
			}
		}

		//开始比对
		//建立表头
		item[0] = "应答器描述项";
		item[1] = "应答器内容";
		item[2] = "数据表内容";
		item[3] = "比对结果";
		compareResultVec.push_back(item);

		for (int i = 0; i < distance_balise.size(); i++, p++)
		{
			if (p >= sectionStartKmVec_sheet.size())
			{
				item[0] = "到特殊轨道区段的距离";
				item[1] = QString::number(distance_balise[i]);
				item[2] = "无匹配";
				item[3] = "不一致";
				compareResultVec.push_back(item);

				item[0] = "特殊轨道区段的长度";
				item[1] = QString::number(length_balise[i]);
				item[2] = "无匹配";
				item[3] = "不一致";
				compareResultVec.push_back(item);
			}
			else
			{
				item[0] = "到特殊轨道区段的距离";
				item[1] = QString::number(distance_balise[i]);
				int brokenLinkLen = JudjeBrokenLink(DesignData::brokenLinkVec, sectionStartKmVec_sheet[p], m_baliseLocation);
				item[2] = QString::number(abs(sectionStartKmVec_sheet[p] - m_baliseLocation) + brokenLinkLen);
				item[3] = item[1] == item[2] ? "一致" : "不一致";
				compareResultVec.push_back(item);

				item[0] = "特殊轨道区段的长度";
				item[1] = QString::number(length_balise[i]);
				item[2] = QString::number(sectionLenVec_sheet[p]);
				item[3] = item[1] == item[2] ? "一致" : "不一致";
				compareResultVec.push_back(item);
			}
		}
		return compareResultVec;
	}
	else
	{
		return compareResultVec;
	}
}

QVector<QVector<QString> > CompareBaliseData_THREAD::Compare_Grade(int position)
{
	QVector<QVector<QString> > compareResultVec;

	QVector<QVector<QString> > resultVec;
	if ((DesignData::gradeUpProVec.empty() && g_LineDirection == UP) || (DesignData::gradeDownProVec.empty() && g_LineDirection == DOWN))//如果没有导入相应的列控数据，那么就退出
		return resultVec;
	bool permission = false; //用来判断是否进行后续对比
	QVector<Item> ETCS_21;
	ETCS_21 = m_baliseData_Package[position];
	//判断数据包验证方向与列车运行方向是否符合
	QVector<Item> Q_DIR_Vec = Analyze_BaliseMessage::FindItem("Q_DIR", ETCS_21);
	if ((Q_DIR_Vec[1].value == 0 && g_TrainDirection == BACK) || (Q_DIR_Vec[1].value == 1 && g_TrainDirection == FRONT)
		|| Q_DIR_Vec[1].value == 2) //如果列车运行方向与信息包验证方向符合才进行后续对比
	{
		permission = true;
	}
	if (permission)
	{
		QVector<Item> D_GRADIENTVec_balise = Analyze_BaliseMessage::FindItem("D_GRADIENT", ETCS_21);
		QVector<Item> Q_GDIRVec_balise = Analyze_BaliseMessage::FindItem("Q_GDIR", ETCS_21);
		QVector<Item> G_AVec_balise = Analyze_BaliseMessage::FindItem("G_A", ETCS_21);

		QVector<int> gradeVec_balise;
		QVector<int> lengthVec_balise;
		//填充坡度-应答器
		int x = 1;
		for (int i = 1; i < G_AVec_balise.size(); i++)
		{
			if (Q_GDIRVec_balise[i].value == 0 && G_AVec_balise[i].value != 0) //0表示下坡或平坡
			{
				x = -1;
			}
			else
			{
				x = 1;
			}
			if (G_AVec_balise[i].value != 255)//坡度描述以G_A = 255结束
			{
				gradeVec_balise.append(G_AVec_balise[i].value*x);
			}
		}

		//填充长度-应答器
		for (int i = 2; i < D_GRADIENTVec_balise.size(); i++)
		{
			lengthVec_balise.append(D_GRADIENTVec_balise[i].value);
		}

		//填充第一项
		QVector<QString> item(4);
		item[0] = m_Balise_IDstr;
		item[1] = QString::number(m_udpDataOrder, 10);
		if (m_originBaliseBitMessage.type &BaliseType::SideLine)
		{
			item[2] = "(侧线)";
		}
		else
		{
			item[3] = "";
		}
		compareResultVec.push_back(item);


		//根据上下行选定工程数据表
		QVector<Grade> gradeVec_temp;
		if (g_LineDirection == UP)
		{
			gradeVec_temp = processVector(DesignData::gradeUpProVec);
		}
		else
		{
			gradeVec_temp = processVector(DesignData::gradeDownProVec);
		}

		//得到表中终点公里标数组
		QVector<int> gradeEndKmVec(gradeVec_temp.size());
		for (int i = 0; i < gradeVec_temp.size(); i++)
		{
			gradeEndKmVec[i] = DesignData::ConvertKmStr2Num(gradeVec_temp[i].endKm);
		}

		//判断公里标增减
		bool kmAddFlag = false;
		if ((g_LineDirection == DOWN && g_TrainDirection == FRONT) ||
			(g_LineDirection == UP && g_TrainDirection == BACK)) //下行正向或者上行反向公里标增大
		{
			kmAddFlag = true;
		}
		else
		{
			kmAddFlag = false;
		}

		//填充第二项
		item[0] = "正线应答器";
		item[1] = QString("应答器位置%1").arg(QString::number(m_baliseLocation));

		//找到运行方向坡度描述起始点前第一个坡度变化点
		//下行正向公里标增大，返回第一个大于或等于num的位置
		//上行正向公里标减小，返回第一个小于或等于num的位置
		int gardeStartKm = kmAddFlag ? m_baliseLocation + D_GRADIENTVec_balise[1].value : m_baliseLocation - D_GRADIENTVec_balise[1].value;

		int p = 0;
		for (int i = 0; i < gradeEndKmVec.size(); i++)
		{
			if (g_LineDirection == DOWN)
			{
				if (gradeEndKmVec[i] >= gardeStartKm)
				{
					p = i;
					break;
				}
			}
			else
			{
				if (gradeEndKmVec[i] <= gardeStartKm)
				{
					p = i;
					break;
				}
			}
		}

		if (p == gradeEndKmVec.size() ||
			(p == 0 && gardeStartKm != gradeEndKmVec.front())) //已经超出工程数据表描述
		{
			item[2] = "";
			item[3] = "已超出工程数据表描述";
			compareResultVec.push_back(item);
		}
		else
		{
			item[2] = QString("前方变坡点位置%1").arg(QString::number(gradeEndKmVec[p]));
			item[3] = "未超出数据表描述";
			compareResultVec.push_back(item);
		}

		//填充表头
		item[0] = "比对项";
		item[1] = "应答器";
		item[2] = "工程数据表";
		item[3] = "比对结果";
		compareResultVec.push_back(item);

		//计算第一段长度和坡度
		int firstLen = 0;
		int firstG_A = 0;
		int brokenLinkLen;

		QVector<int> gradeVec_sheet;
		QVector<int> lengthVec_sheet;
		if (m_baliseLocation != -1) {
			if (g_TrainDirection == BACK)
			{
				if (p - 1 >= 0)
				{
					firstG_A = floor(gradeVec_temp[p].grade.toDouble()*-1);  //反向坡度需取反
					brokenLinkLen = JudjeBrokenLink(DesignData::brokenLinkVec, gardeStartKm, gradeVec_temp[p - 1].endKm.toInt());
					firstLen = abs(gardeStartKm - gradeVec_temp[--p].endKm.toInt()) + brokenLinkLen;
					gradeVec_sheet.push_back(firstG_A);
					lengthVec_sheet.push_back(firstLen);
					for (int i = p; i >= 0; i--)
					{
						gradeVec_sheet.push_back(floor(gradeVec_temp[i].grade.toDouble()*-1));
						lengthVec_sheet.push_back(gradeVec_temp[i].len.toInt());
					}
				}

			}
			else //正向
			{
				if (gardeStartKm == gradeVec_temp[p].endKm.toInt())
				{
					if (p + 1 < gradeVec_temp.size())
					{
						firstG_A = floor(gradeVec_temp[++p].grade.toDouble());
						firstLen = gradeVec_temp[++p].len.toInt();
						gradeVec_sheet.push_back(firstG_A);
						lengthVec_sheet.push_back(firstLen);
						for (int i = p; i < gradeVec_temp.size(); i++)
						{
							gradeVec_sheet.push_back(floor(gradeVec_temp[i].grade.toDouble()));
							lengthVec_sheet.push_back(gradeVec_temp[i].len.toInt());
						}
					}

				}
				else
				{
					firstG_A = floor(gradeVec_temp[p].grade.toDouble());
					brokenLinkLen = JudjeBrokenLink(DesignData::brokenLinkVec, gardeStartKm, gradeVec_temp[p].endKm.toInt());
					firstLen = abs(gardeStartKm - gradeVec_temp[p++].endKm.toInt());
					gradeVec_sheet.push_back(firstG_A);
					lengthVec_sheet.push_back(firstLen);
					for (int i = p; i < gradeVec_temp.size(); i++)
					{
						gradeVec_sheet.push_back(floor(gradeVec_temp[i].grade.toDouble()));
						lengthVec_sheet.push_back(gradeVec_temp[i].len.toInt());
					}
				}

			}
		}
		//进行后续比对
		int now_index = 0;
		for (int i = 0; i < gradeVec_balise.size(); i++)
		{
			if (gradeVec_sheet.size() - now_index != 0) //工程数据表中至少还剩一个元素
			{
				if (lengthVec_balise[i] <= lengthVec_sheet[now_index]) //不存在坡度合并
				{
					item[0] = "坡度";
					item[1] = QString::number(gradeVec_balise[i]);
					item[2] = QString::number(gradeVec_sheet[now_index]);
					item[3] = item[1] == item[2] ? "一致" : "不一致";
					compareResultVec.push_back(item);

					if (i == gradeVec_balise.size() - 1)
					{
						item[0] = "长度";
						item[1] = QString::number(lengthVec_balise[i]);
						item[2] = QString::number(lengthVec_sheet[now_index]);
						item[3] = lengthVec_balise[i] <= lengthVec_sheet[now_index] ? "在范围内" : "不一致";
						compareResultVec.push_back(item);
					}
					else
					{
						item[0] = "长度";
						item[1] = QString::number(lengthVec_balise[i]);
						item[2] = QString::number(lengthVec_sheet[now_index]);
						item[3] = item[1] == item[2] ? "一致" : "不一致";
						compareResultVec.push_back(item);
					}
				}
				else //可能存在坡度合并
				{
					int grade_Combine = 255;
					int length_Combine = -1;
					judjeGradeCombine(gradeVec_sheet, lengthVec_sheet, now_index, gradeVec_balise[i], lengthVec_balise[i], grade_Combine, length_Combine);
					if (length_Combine != -1)//存在坡度合并
					{
						item[0] = "坡度";
						item[1] = QString::number(gradeVec_balise[i]);
						item[2] = QString("合并坡度%1").arg(QString::number(grade_Combine));
						item[3] = gradeVec_balise[i] == grade_Combine ? "一致" : "不一致";
						compareResultVec.push_back(item);

						if (i == gradeVec_balise.size() - 1) //最后一个需要特殊处理
						{
							item[0] = "长度";
							item[1] = QString::number(lengthVec_balise[i]);
							item[2] = QString("合并长度%1").arg(QString::number(length_Combine));
							item[3] = lengthVec_balise[i] <= length_Combine ? "在范围内" : "不一致";
							compareResultVec.push_back(item);
						}
						else
						{
							item[0] = "长度";
							item[1] = QString::number(lengthVec_balise[i]);
							item[2] = QString("合并长度%1").arg(QString::number(length_Combine));
							item[3] = lengthVec_balise[i] == length_Combine ? "一致" : "不一致";
							compareResultVec.push_back(item);
						}
					}
					else
					{
						item[0] = "坡度";
						item[1] = QString::number(gradeVec_balise[i]);
						item[2] = QString::number(gradeVec_sheet[now_index]);
						item[3] = item[1] == item[2] ? "一致" : "不一致";
						compareResultVec.push_back(item);

						item[0] = "长度";
						item[1] = QString::number(lengthVec_balise[i]);
						item[2] = QString::number(lengthVec_sheet[now_index]);
						item[3] = item[1] == item[2] ? "一致" : "不一致";
						compareResultVec.push_back(item);
					}
				}
				now_index++;
			}
			else //工程数据表已用完
			{
				item[0] = "坡度";
				item[1] = QString::number(gradeVec_balise[i]);
				item[2] = "无匹配";
				item[3] = "不一致";
				compareResultVec.push_back(item);

				item[0] = "长度";
				item[1] = QString::number(lengthVec_balise[i]);
				item[2] = "无匹配";
				item[3] = "不一致";
				compareResultVec.push_back(item);
			}

		}
	}
	return compareResultVec;
}

void CompareBaliseData_THREAD::judjeGradeCombine(QVector<int> &gradeVec_sheet, QVector<int> &lengthVec_sheet, int& now_index, const int grade_balise, const int len_balise, int &grade_combine, int &len_combine)
{
	//grade_combine = 255;
	len_combine = -1;
	//找出第一个和应答器坡度方向不一致的位置
	int end_index = now_index;
	for (int i = now_index; i < gradeVec_sheet.size(); i++)
	{
		if ((grade_balise >= 0 && gradeVec_sheet[i] >= 0) || (grade_balise < 0 && gradeVec_sheet[i] < 0))
		{
			continue;
		}
		else
		{
			end_index = i;
			break;
		}
	}

	//逐一试探是否有坡度合并
	bool hasCombine;
	for (int i = now_index, sum = 0; i < end_index; i++)
	{
		sum += lengthVec_sheet[i];
		if (sum == len_balise)
		{
			hasCombine = true;
			grade_combine = gradeVec_sheet[now_index];
			//计算合并坡度值
			for (int j = now_index; j < i; j++)
			{

				grade_combine = gradeVec_sheet[j + 1] >= 0 ? max(grade_combine, gradeVec_sheet[j + 1]) : //上坡合并取最大
					-max(abs(grade_combine), abs(gradeVec_sheet[j + 1]));
			}
			len_combine = sum;
			now_index = i;
			break;
		}
	}



}

void CompareBaliseData_THREAD::judjeTrackCircuitCombine(QVector<PathWayData_Less> &pathWayData_LessVec, int &now_index, const int & length_balise, const QString & signalType_balise, const QString & fre_balise, int & length_combine, QString & signalType_combine, QString & fre_combine)
{
	//寻找从now_Index开始和应答器中该位置描述的载频和信号机类型都不相同的第一个索引位置

	//合并规则：1.一个闭塞分区内，如果多个相邻区段的载频相同，则这些轨道区段可以合并
	//          2.当应答器组描述的数据超出应答器容量后，如果通过增加应答器组内数量无法解决，在丢失一组应答器不影响列车运行的条件下，可对制动距离范围内由远及近对各闭塞分区内的轨道区段进行合并，合并后的各闭塞分区载频为“无载频”。
	int end_index = pathWayData_LessVec.size();
	QString firstFre = pathWayData_LessVec[now_index].pathWayFre;
	for (int i = now_index; i < pathWayData_LessVec.size(); i++)
	{
		if (fre_balise != "0")  //不是远端合并，需要判断相邻区段载频是否相等
		{
			if (pathWayData_LessVec[i].pathWayFre != firstFre)
			{
				end_index = i;
				break;
			}
		}
	}

	//开始判断是否存在轨道电路合并
	for (int i = now_index, sum = 0; i < end_index; i++)
	{
		sum += pathWayData_LessVec[i].pathWayLen;
		if (sum == length_balise) //找到合并
		{
			length_combine = sum;
			signalType_combine = pathWayData_LessVec[i].signalSpotType;
			fre_combine = fre_balise == "0" ? "0" : pathWayData_LessVec[i].pathWayFre;//远端合并载频为0
			now_index = i;
			break;
		}
	}
}





QVector<QVector<QString> > CompareBaliseData_THREAD::Compare_Speed(int position)
{
	QVector<QVector<QString> > compareResultVec;
	if ((DesignData::pathWaySpeedUpVec.empty() && g_LineDirection == UP) || (DesignData::pathWaySpeedDownVec.empty() && g_LineDirection == DOWN))//如果没有导入相应的列控数据，那么就退出
		return compareResultVec;
	bool permission = false; //用来判断是否进行后续对比
	QVector<Item> ETCS_27;
	ETCS_27 = m_baliseData_Package[position];
	//判断数据包验证方向与列车运行方向是否符合
	QVector<Item> Q_DIR_Vec = Analyze_BaliseMessage::FindItem("Q_DIR", ETCS_27);
	if ((Q_DIR_Vec[1].value == 0 && g_TrainDirection == BACK) || (Q_DIR_Vec[1].value == 1 && g_TrainDirection == FRONT)
		|| Q_DIR_Vec[1].value == 2) //如果列车运行方向与信息包验证方向符合才进行后续对比
	{
		permission = true;
	}
	if (permission)
	{
		QVector<QString> item(4);
		item[0] = m_Balise_IDstr;
		item[1] = QString::number(m_udpDataOrder, 10);
		if (m_originBaliseBitMessage.type &BaliseType::SideLine)
		{
			item[2] = "(侧线)";
		}
		else
		{
			item[2] = "";
		}
		compareResultVec.push_back(item);


		QVector<Item> D_STATICVec_balise = Analyze_BaliseMessage::FindItem("D_STATIC", ETCS_27);
		QVector<Item> V_STATICVec_balise = Analyze_BaliseMessage::FindItem("V_STATIC", ETCS_27);
		QVector<Item> V_ITER_balise = Analyze_BaliseMessage::FindItem("V_ITER", ETCS_27);

		QVector<QString> speedVec_balise;
		QVector<QString> lengthVec_balise;


		//填充速度-应答器
		for (int i = 1; i < V_STATICVec_balise.size(); i++)
		{
			if (V_STATICVec_balise[i].value != 127)//速度描述以V_STATIC = 127结束
			{
				speedVec_balise.append(QString::number(V_STATICVec_balise[i].value * 5, 10)); //速度以5km/h为分辨率
			}
		}
		//填充长度-应答器
		for (int i = 2; i < D_STATICVec_balise.size(); i++)
		{
			lengthVec_balise.append(QString::number(D_STATICVec_balise[i].value, 10));
		}

		if (!SideLineFlag)//正线比对逻辑
		{
			//根据线别和运行方向选定工程数据表，并判别公里标增减
			bool kmAddFlag = false;
			QVector<PathWaySpeed> pathWaySpeedVec_temp;
			if (g_LineDirection == UP)
			{
				if (g_TrainDirection == FRONT)
				{
					kmAddFlag = false;
					pathWaySpeedVec_temp = processVector(DesignData::pathWaySpeedUpVec);
				}
				else
				{
					kmAddFlag = true;
					pathWaySpeedVec_temp = processVector(DesignData::pathWaySpeedUpBackVec);
				}
			}
			else
			{
				if (g_TrainDirection == FRONT)
				{
					kmAddFlag = true;
					pathWaySpeedVec_temp = processVector(DesignData::pathWaySpeedDownVec);
				}
				else
				{
					kmAddFlag = false;
					pathWaySpeedVec_temp = processVector(DesignData::pathWaySpeedDownBackVec);
				}
			}

			int location = 0;
			if (D_STATICVec_balise.size() != 0)
			{
				location = D_STATICVec_balise[1].value;
			}
			location = kmAddFlag ? location : -location;
			int position = BinarySearchPathWaySpeed(pathWaySpeedVec_temp, m_baliseLocation + location, kmAddFlag);

			item[0] = "正线应答器";
			//第一项应答器所在公里标和其后方第一个速度变化点
			item[1] = QString("应答器位置%1").arg(QString::number(m_baliseLocation));
			if (position == -1) //应答器位置后方在工程数据表中速度变化点
			{
				item[2] = QString("后方%1").arg("无变速点");
				item[3] = "未超出数据表描述";
			}
			else if (position >= pathWaySpeedVec_temp.size() - 1 || m_baliseLocation == -1) //应答器位置已超出工程数据表描述
			{
				item[2] = QString("后方变速点位置%1").arg(pathWaySpeedVec_temp[position].endKm);
				item[3] = "已超出数据表描述";
				compareResultVec.push_back(item);
				pathWaySpeedVec_temp.clear();
			}
			else
			{
				item[2] = QString("后方变速点位置%1").arg(pathWaySpeedVec_temp[position].endKm);
				item[3] = "未超出数据表描述";
			}
			compareResultVec.push_back(item);

			//建立表头
			item[0] = "应答器描述项";
			item[1] = "应答器内容";
			item[2] = "数据表内容";
			item[3] = "比对结果";
			compareResultVec.push_back(item);

			int firstLen = 0;
			if (position < pathWaySpeedVec_temp.size() - 1) {
				//计算速度信息起始点到下一个变速点的距离,公里标加减,需考虑长短链
				int nextPositionKm = DesignData::ConvertKmStr2Num(pathWaySpeedVec_temp[position + 1].endKm);
				int brokenLinkLen = JudjeBrokenLink(DesignData::brokenLinkVec, m_baliseLocation, nextPositionKm);
				firstLen = abs(nextPositionKm - (m_baliseLocation + location)) + brokenLinkLen;
			}
			
			for (int i = 0; i < speedVec_balise.size(); i++, position++)
			{
				if (position + 1 >= pathWaySpeedVec_temp.size())
				{

					//第一项 速度
					item[0] = "允许速度";
					item[1] = speedVec_balise[i];
					item[2] = "无匹配";
					item[3] = "不一致";
					compareResultVec.push_back(item);

					//第二项 长度
					item[0] = "距离增量";
					item[1] = lengthVec_balise[i];
					item[2] = "无匹配";
					item[3] = "不一致";
					compareResultVec.push_back(item);
				}
				else
				{
					//第一项 速度
					item[0] = "允许速度";
					item[1] = speedVec_balise[i];
					item[2] = pathWaySpeedVec_temp[position + 1].speed;
					item[3] = item[1] == item[2] ? "一致" : "不一致";
					compareResultVec.push_back(item);

					//第二项 长度
					if (i == speedVec_balise.size() - 1) //最后一项长度只要小于区段长度就好
					{
						item[0] = "距离增量";
						item[1] = lengthVec_balise[i];
						if (i == 0)
						{
							item[2] = QString::number(firstLen);
						}
						else
						{
							item[2] = pathWaySpeedVec_temp[position + 1].len;
						}
						if (item[1].toInt() < item[2].toInt())
						{
							item[3] = "在终点范围内";
						}
						else if (item[1].toInt() == item[2].toInt())
						{
							item[3] = "一致";
						}
						else
						{
							item[3] = "不一致";
						}
					}
					else
					{
						item[0] = "距离增量";
						item[1] = lengthVec_balise[i];

						if (i == 0)
						{
							item[2] = QString::number(firstLen);
						}
						else
						{
							item[2] = pathWaySpeedVec_temp[position + 1].len;
						}

						item[3] = item[1] == item[2] ? "一致" : "不一致";
					}
					compareResultVec.push_back(item);
				}

			}
		}
		else //侧线比对逻辑
		{
			//填充其他信息
			item[0] = "侧线应答器";
			item[1] = "";
			item[2] = "";
			item[3] = "";
			compareResultVec.push_back(item);

			//填充表头
			item[0] = "应答器描述项";
			item[1] = "应答器内容";
			item[2] = "数据表内容";
			item[3] = "比对结果";
			compareResultVec.push_back(item);

			//首先由进路数据表获取侧线数据
			QVector<int> sideLineSpeedVec = SideLineInfo.speed;
			QVector<int> sideSpeedLenVec = SideLineInfo.speed_Length;




			static int s_D_STATIC = 0;
			int sideLineIn_Index = 0;
			enum SideLineBaliseType { sideLine_begin = 0, sideLine_in, sideLine_end };
			SideLineBaliseType sideLineBaliseType = SideLineBaliseType::sideLine_begin;
			for (int i = 0; i < SideLineInfo.balise_ID.size(); i++)
			{
				if (m_Balise_IDstr.contains(SideLineInfo.balise_ID[i]))
				{
					sideLineBaliseType = sideLine_in;
					sideLineIn_Index = i;
					break;
				}
			}
			if (sideLineBaliseType == sideLine_begin)  //009
			{
				//存储D_SIGNAL用于确定侧线内应答器位置
				s_D_STATIC = D_STATICVec_balise[1].value;

				//先将长度合并,以判断可能会出现的合并情况,并给出合并后安全侧速度;
				QVector<int> combineLenVec;
				QVector<int> combineSpeedVec;

				if (sideLineSpeedVec.size() > 1) //两个及以上才进行合并
				{
					int sumLen_temp = sideSpeedLenVec[0];
					int minSpeed = sideLineSpeedVec[0];
					for (int i = 1; i < sideSpeedLenVec.size(); i++)
					{
						sumLen_temp += sideSpeedLenVec[i];
						combineLenVec.push_back(sumLen_temp);
						if (minSpeed > sideLineSpeedVec[i])
						{
							minSpeed = sideLineSpeedVec[i];
						}
						combineSpeedVec.push_back(minSpeed);
					}
				}

				//第一项
				item[0] = "到本应答器所描述速度信息起始点距离";
				item[1] = QString::number(s_D_STATIC);
				item[2] = "无法获得";
				item[3] = "";
				compareResultVec.push_back(item);

				for (int i = 0; i < speedVec_balise.size(); i++)
				{
					if (i >= sideLineSpeedVec.size())
					{
						//第一项信号机类型
						item[0] = "速度";
						item[1] = speedVec_balise[i];
						item[2] = "无匹配";
						item[3] = "不一致";
						compareResultVec.push_back(item);

						//第二项长度
						item[0] = "长度";
						item[1] = lengthVec_balise[i];
						item[2] = "无匹配";
						item[3] = "不一致";
						compareResultVec.push_back(item);
					}
					else
					{
						//可能有合并情况
						bool combineFlag = false;
						for (int j = 0; j < combineLenVec.size(); j++)
						{
							if (lengthVec_balise[i].toInt() == combineLenVec[j]) //存在合并
							{
								combineFlag = true;
								//第一项
								item[0] = "合并速度";
								item[1] = speedVec_balise[i];
								item[2] = QString::number(combineSpeedVec[j]);
								item[3] = item[1] == item[2] ? "一致" : "不一致";
								compareResultVec.push_back(item);

								//第二项长度
								item[0] = "合并长度";
								item[1] = lengthVec_balise[i];
								item[2] = QString::number(combineLenVec[j]);
								item[3] = item[1] == item[2] ? "一致" : "不一致";
								compareResultVec.push_back(item);
								break;
							}
						}
						if (!combineFlag)
						{
							//第一项
							item[0] = "速度";
							item[1] = speedVec_balise[i];
							item[2] = QString::number(sideLineSpeedVec[i]);
							item[3] = item[1] == item[2] ? "一致" : "不一致";
							compareResultVec.push_back(item);

							//第二项长度
							item[0] = "长度";
							item[1] = lengthVec_balise[i];
							item[2] = QString::number(sideSpeedLenVec[i]);
							item[3] = item[1] == item[2] ? "一致" : "不一致";
							compareResultVec.push_back(item);
						}
					}
				}
			}
			if (sideLineBaliseType == sideLine_in) //071
			{
				//第一项
				item[0] = "到本应答器所描述速度信息起始点距离";
				item[1] = QString::number(D_STATICVec_balise[1].value);
				item[2] = "无法获得";
				item[3] = "";
				compareResultVec.push_back(item);

				//定标侧线内应答器所描述速度信息起始点与速度区段起始点的相对位置
				int sumBalise_Distance = 0;

				for (int i = 0; i <= sideLineIn_Index; i++)
				{
					sumBalise_Distance += SideLineInfo.balise_Diatance[i];
				}
				int sideLineInPos = sumBalise_Distance - s_D_STATIC + D_STATICVec_balise[1].value;
				//锁定应答器所在速度区段
				int sumLen = 0;
				int sectionIndex = -1;
				int sectionIndexLen = 0;
				for (int i = 0; i < sideSpeedLenVec.size(); i++)
				{
					sumLen += sideSpeedLenVec[i];
					if (sideLineInPos <= sumLen)
					{
						sectionIndex = i;
						sectionIndexLen = sumLen;
						break;
					}
				}
				if (sectionIndex == -1)
				{
					for (int i = 0; i < speedVec_balise.size(); i++)
					{
						//第一项
						item[0] = "速度";
						item[1] = speedVec_balise[i];
						item[2] = "无匹配";
						item[3] = "不一致";
						compareResultVec.push_back(item);

						//第二项长度
						item[0] = "长度";
						item[1] = lengthVec_balise[i];
						item[2] = "无匹配";
						item[3] = "不一致";
						compareResultVec.push_back(item);
					}
				}
				else
				{

					//先将长度合并,以判断可能会出现的合并情况,并给出合并后安全侧速度;
					QVector<int> combineLenVec;
					QVector<int> combineSpeedVec;

					if (sideLineSpeedVec.size() > 1) //两个及以上才进行合并
					{
						int sumLen_temp = sectionIndexLen - sideLineInPos;
						int minSpeed = sideLineSpeedVec[sectionIndex];
						for (int i = sectionIndex + 1; i < sideSpeedLenVec.size(); i++)
						{
							sumLen_temp += sideSpeedLenVec[i];
							combineLenVec.push_back(sumLen_temp);
							if (minSpeed > sideLineSpeedVec[i])
							{
								minSpeed = sideLineSpeedVec[i];
							}
							combineSpeedVec.push_back(minSpeed);
						}
					}

					for (int i = 0; i < speedVec_balise.size(); i++, sectionIndex++)
					{
						if (sectionIndex >= sideLineSpeedVec.size())
						{
							//第一项信号机类型
							item[0] = "速度";
							item[1] = speedVec_balise[i];
							item[2] = "无匹配";
							item[3] = "不一致";
							compareResultVec.push_back(item);

							//第二项长度
							item[0] = "长度";
							item[1] = lengthVec_balise[i];
							item[2] = "无匹配";
							item[3] = "不一致";
							compareResultVec.push_back(item);
						}
						else
						{
							//可能有合并情况
							bool combineFlag = false;
							for (int j = 0; j < combineLenVec.size(); j++)
							{
								if (lengthVec_balise[i].toInt() == combineLenVec[j]) //存在合并
								{
									combineFlag = true;
									//第一项信号机类型
									item[0] = "合并速度";
									item[1] = speedVec_balise[i];
									item[2] = QString::number(combineSpeedVec[j]);
									item[3] = item[1] == item[2] ? "一致" : "不一致";
									compareResultVec.push_back(item);

									//第二项长度
									item[0] = "合并长度";
									item[1] = lengthVec_balise[i];
									item[2] = QString::number(combineLenVec[j]);
									item[3] = item[1] == item[2] ? "一致" : "不一致";
									compareResultVec.push_back(item);
									break;
								}
							}
							if (!combineFlag)
							{
								//第一项信号机类型
								item[0] = "速度";
								item[1] = speedVec_balise[i];
								item[2] = QString::number(sideLineSpeedVec[sectionIndex]);
								item[3] = item[1] == item[2] ? "一致" : "不一致";
								compareResultVec.push_back(item);

								//第二项长度
								item[0] = "长度";
								item[1] = lengthVec_balise[i];
								if (i == 0)
								{
									item[2] = QString::number(sectionIndexLen - sideLineInPos);
								}
								else
								{
									item[2] = QString::number(sideSpeedLenVec[sectionIndex]);
								}

								item[3] = item[1] == item[2] ? "一致" : "不一致";
								compareResultVec.push_back(item);
							}
						}
					}
				}

			}
		}
		return compareResultVec;
	}
	else
	{
		return compareResultVec;
	}
}


QVector<QVector<QString> > CompareBaliseData_THREAD::Compare_TrackTemporarySpeedLimit(int position)
{
	QVector<QVector<QString> > resultVec;

	if ((DesignData::pathWayDataUpProVec.empty() && g_LineDirection == UP && g_TrainDirection == FRONT) || (DesignData::pathWayDataUpBackVec.empty() && g_LineDirection == UP && g_TrainDirection == BACK) || (DesignData::pathWayDataDownProVec.empty() && g_LineDirection == DOWN && g_TrainDirection == FRONT) || (DesignData::pathWayDataDownBackVec.empty() && g_LineDirection == DOWN && g_TrainDirection == BACK))//如果没有导入相应的列控数据，那么就退出
		return resultVec;
	bool permission = false; //用来判断是否进行后续对比
	QVector<Item> ETCS_44;
	ETCS_44 = m_baliseData_Package[position];
	QVector<Item> NID_XUSER_Vec = Analyze_BaliseMessage::FindItem("NID_XUSER", ETCS_44);
	if (NID_XUSER_Vec[1].value != 2) //没有CTCS2包
	{
		return resultVec;
	}
	//有CTCS2包，继续判断数据包验证方向与列车运行方向是否符合
	QVector<Item> Q_DIR_Vec = Analyze_BaliseMessage::FindItem("Q_DIR", ETCS_44);
	if ((!Q_DIR_Vec[1].value && g_TrainDirection == BACK) || (Q_DIR_Vec[1].value == 1 && g_TrainDirection == FRONT)
		|| Q_DIR_Vec[1].value == 2) //如果列车运行方向与信息包验证方向符合才进行后续对比
	{
		permission = true;
	}
	if (permission)
	{
		//用FindItem获取应答器 到临时限速区距离
		QVector<Item> D_TSRVec_balise_temp = Analyze_BaliseMessage::FindItem("D_TSR", ETCS_44);
		QVector<Item> L_TSRVec_balise_temp = Analyze_BaliseMessage::FindItem("L_TSR", ETCS_44);
		QVector<Item> V_TSRVec_balise_temp = Analyze_BaliseMessage::FindItem("V_TSR", ETCS_44);

		QVector<QString> startVec_balise;
		QVector<QString> endVec_balise;
		QVector<QString> speedVec_balise;
		int startPos = 0;
		int endPos = m_baliseLocation;
		if ((g_LineDirection == DOWN && g_TrainDirection == FRONT) ||
			(g_LineDirection == UP && g_TrainDirection == BACK)) //下行正向或者上行反向公里标增大
		{
			for (int i = 1; i < D_TSRVec_balise_temp.size(); ++i)
			{
				startPos = endPos + D_TSRVec_balise_temp[i].value;
				startVec_balise.append(QString::number(startPos, 10));
				endPos = startPos + L_TSRVec_balise_temp[i].value;
				endVec_balise.append(QString::number(endPos, 10));
				speedVec_balise.append(QString::number(V_TSRVec_balise_temp[i].value, 10));
			}
		}
		else
		{
			for (int i = 1; i < D_TSRVec_balise_temp.size(); ++i)
			{
				startPos = endPos - D_TSRVec_balise_temp[i].value;
				startVec_balise.append(QString::number(startPos, 10));
				endPos = startPos - L_TSRVec_balise_temp[i].value;
				endVec_balise.append(QString::number(endPos, 10));
				speedVec_balise.append(QString::number(V_TSRVec_balise_temp[i].value, 10));
			}
		}
		QVector<QString> forHistoryInfoVec;
		forHistoryInfoVec.append(m_Balise_IDstr);
		forHistoryInfoVec.append(QString::number(m_udpDataOrder, 10));
		if (m_originBaliseBitMessage.type & BaliseType::SideLine)
		{
			forHistoryInfoVec.push_back("(侧线)");
		}
		else
		{
			forHistoryInfoVec.push_back("");
		}
		resultVec.append(forHistoryInfoVec);
		resultVec.append(startVec_balise);
		resultVec.append(endVec_balise);
		resultVec.append(speedVec_balise);
		return resultVec;
	}
	else
	{
		return resultVec;
	}
}

/******************************************************************
* @功能：对于输入的字符串进行过滤，滤除不必要的字符，用于在工程数据表导入数据的时候
* @形参：QString &input 要过滤的字符串,
         QString& filter filter中的字符用于保留或者滤除，
         FilterType filtertype:None，表示不需要额外的常规字符集补充
                               Num，在要保留或者删除的字符集中加入所有数字
                               LowerLetter,在要保留或者删除的字符集中加入所有小写字母
                               UpperLetter,在要保留或者删除的字符集中加入所有大写字母
                               Chinese,在要保留或者删除的字符集中加入所有中文字符
         flag为true时，filter中的字符用于滤除，为false时，用于保留
* @返回值：过滤后的字符串
* @作者：txw
* @修改日期：2018-01-30
*******************************************************************/
QString CompareBaliseData_THREAD::strFilter(const QString &input, const QString& filter, FilterType filtertype, bool flag)
{
    QString ret;
    for (int i = 0; i < input.size(); i++)
    {
        QChar ch = 0;
        if ((filtertype & Num) != 0)
        {
            if (input[i] >= '0'&&input[i] <= '9')
                ch = input[i];
        }
        if ((filtertype & LowerLetter) != 0)
        {
            if (input[i] >= 'a'&&input[i] <= 'z')
                ch = input[i];
        }
        if ((filtertype & UpperLetter) != 0)
        {
            if (input[i] >= 'A'&&input[i] <= 'Z')
                ch = input[i];
        }
        if ((filtertype & Chinese) != 0)
        {
            ushort uni = input[i].unicode();
            if (uni >= 0x4E00 && uni <= 0x9FBF)
                ch = input[i];
        }
        if (ch == nullptr)
        {
            for (int j = 0; j < filter.size(); j++)
            {
                if (input[i] == filter[j])
                {
                    ch = input[i];
                    break;
                }
            }
        }
        if (flag)//滤除
        {
            if (ch == nullptr)
                ret.push_back(input[i]);
        }
        else//保留
        {
            if (ch != nullptr)
                ret.push_back(input[i]);
        }
    }
    return ret;
}



