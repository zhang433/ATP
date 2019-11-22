#include "baliselocation.h"
#include <QObject>
#include <QQueue>
#include <QVector>
#include "basestructer.h"
#include "designdata.h"
#include <QDebug>
#include <algorithm>
#include <assert.h>
extern Train_Dir g_TrainDirection;//列车运行方向
extern Line_Dir g_LineDirection;//列车运行线别

AccessRode_MapType BaliseLocation::SideLineInfoIn,BaliseLocation::SideLineInfoOut;

BaliseLocation::BaliseLocation()
{

}

/**
 * @brief BaliseLocation::LinkedBliseGroupID 按顺序获取本应答器的链接应答器组的编号
 * @param BPK 解析后的应答器报文
 * @return 本应答器的链接应答器组的编号，没有E5包和链接应答器时返回空的QList
 */
QList<Record_Baliseposition> BaliseLocation::LinkedBliseGroupID(const BalisePackCollector& BPK)
{
    QList<Record_Baliseposition> queue;
    QVector<int> Index;
    Index = Analyze_BaliseMessage::GetPackagePosition("ETCS-5", BPK);
    if (!Index.isEmpty())
    {
        BalisePackage ETCS_5;
        for (int i = 0; i < Index.size(); i++)//寻找与本次运行方向相符合的ETCS-5包
        {
            if ((BPK[Index[i]][2].value == g_TrainDirection) || (BPK[Index[i]][2].value == 2))
            {
                ETCS_5 = BPK[Index[i]];
                break;
            }
        }
        if(!ETCS_5.isEmpty())
        {
            double scale = 0.0;
            switch (ETCS_5[4].value)
            {
            case 0:scale = 0.1; break;
            case 1:scale = 1; break;
            case 2:scale = 10; break;
            default:break;
            }
            /********************************************下面根据ETCS5包的内容更新要比对的应答器列表*****************************************/
            BalisePackage D_LINK_ItemCollector = Analyze_BaliseMessage::FindItem(QString("D_LINK"), ETCS_5);//按顺序获取该信息包中所有包含“D_LINK”字符的项
            BalisePackage Temp_NID_C_ItemCollector = Analyze_BaliseMessage::FindItem("NID_C", ETCS_5);//不是ETCS5包中描述的每一个新的应答器组都具有地区编号，要视Q_NEWCOUNTER的值而定，因此这个变量还应再进行进一步的处理
            Temp_NID_C_ItemCollector.pop_front();
            BalisePackage NID_C_ItemCollector;//这是处理后的每一个新的应答器组的地区编号
            NID_C_ItemCollector.push_back(BPK[0][8]);//第一项用于记录本项的值
            BalisePackage NID_BG_ItemCollector = Analyze_BaliseMessage::FindItem("NID_BG", ETCS_5);//按顺序获取该信息包中所有包含“NID_BG”字符的项
            BalisePackage Q_NEWCOUNTER_ItemCollector = Analyze_BaliseMessage::FindItem("Q_NEWCOUNTRY", ETCS_5);//按顺序获取该信息包中所有包含“Q_NEWCOUNTER”字符的项
            BalisePackage Q_LOCACC_ItemCollector = Analyze_BaliseMessage::FindItem("Q_LOCACC", ETCS_5);//按顺序获取该信息包中所有包含“Q_LOCACC”字符的项
            BalisePackage Q_LINKORIENTATION_ItemCollector = Analyze_BaliseMessage::FindItem("Q_LINKORIENTATION", ETCS_5);//按顺序获取该信息包中所有包含“NEW_COUNTER”字符的项
            if (Q_NEWCOUNTER_ItemCollector.size() != NID_BG_ItemCollector.size() || NID_BG_ItemCollector.size() != Q_LINKORIENTATION_ItemCollector.size())
            {
                qDebug() << ("应答器E5包链接关系错误:NEW_COUNTER项的个数与NID_BG项的个数不匹配");
                return queue;
            }
            for (int i = 1; i < Q_NEWCOUNTER_ItemCollector.size(); i++)
            {
                if (Q_NEWCOUNTER_ItemCollector[i].value == 0)//如果是0，表示这一组的应答器组的地区编号与上一组应答器组的地区编号相同
                {
                    if (!NID_C_ItemCollector.size())//如果NID_C_ItemCollector是空的
                        NID_C_ItemCollector.push_back(BPK[0][8]);//把本次接收到的应答器的地区编号作为这一组应答器的地区编号
                    else//否则，这一组应答器组的地区编号与上一组是一样的，复制上一组的地区编号
                        NID_C_ItemCollector.push_back(NID_C_ItemCollector.back());
                }
                else//否则，表示这一组的应答器具有一个新的地区编号的值
                {
                    NID_C_ItemCollector.push_back(Temp_NID_C_ItemCollector.front());
                    Temp_NID_C_ItemCollector.pop_front();
                }
            }
            qint64 nid_c, nid_bg, d_link, allowerr;
            Record_Baliseposition RB;
            queue.clear();//即便旧的应答器位置比对列表还存在比对项也全部清空，以新的应答器中的ETCS5包为基准
            for (int i = 1; i < D_LINK_ItemCollector.size(); i++)
            {
                nid_c = NID_C_ItemCollector[i].value;
                nid_bg = NID_BG_ItemCollector[i].value;
                d_link = D_LINK_ItemCollector[i].value * scale;
                allowerr = Q_LOCACC_ItemCollector[i].value;
                RB.ID = QString("%1").arg((nid_c >> 3) & 0x0000007F, 3, 10, QChar('0')); //大区编号
                RB.ID += "-";
                RB.ID += QString::number((nid_c) & 0x00000007);//分区编号
                RB.ID += "-";
                RB.ID += QString("%1").arg((nid_bg >> 8) & 0x0000003F, 2, 10, QChar('0')); //车站编号
                RB.ID += "-";
                RB.ID += QString("%1").arg((nid_bg) & 0x000000FF, 3, 10, QChar('0')); //车站编号
                RB.D_LINK = d_link;
                RB.AllowErr = allowerr;

                if (!Q_LINKORIENTATION_ItemCollector.isEmpty())
                {
                    RB.Dir = Train_Dir(Q_LINKORIENTATION_ItemCollector.front().value);
                    Q_LINKORIENTATION_ItemCollector.pop_front();
                }
                else
                    qDebug() << ("应答器E5包链接关系错误:Q_LINKORIENTATION项的个数少于预期");
                queue.push_back(RB);
            }//至此，更新了要比对的应答器列表
        }
    }
    return queue;
}

/**
 * @brief BaliseLocation::JudgeSideLineBaliseGroup 判断各应答器在侧线的分布关系
 * @param Balises 一系列的应答器组的编号
 * @param index 当前应答器组的位置
 * @return 返回对应的一系列应答器组的线路属性，如[Normal,Normal,Normal,SideLineIn,SideLine,SideLine,SideLineOut,Normal,Normal]
 */
QList<LineType> BaliseLocation::JudgeSideLineBaliseGroup(const QList<QString>& Balises,int index)
{
    QList<LineType> ret;
    QMap<int,AccessRode_MapType> index2SideLineInfo_table;
    LineType status = LineType::Normal;
    for(int i=0;i<Balises.size();++i)
    {
        auto iter = DesignData::accessRodeMap.equal_range(Balises[i]);
        if(iter.first == iter.second)//在进路表里没有找到以这个应答器组为起点的信息，那么延续之前的状态标识
        {
            ret.push_back(status);
            continue;
        }
        if(Balises.size()<=i+1)//该应答器组的下一组应答器链接为空，那么延续之前的状态标识
        {
            ret.push_back(status);
            continue;
        }
        for(auto iter2=iter.first;iter2!=iter.second;iter2++)
        {
            if(!iter2->balise_ID.isEmpty())//应答器链接关系不为空的情况下
            {
                if(iter2->balise_ID[0]==Balises[i+1])
                {
                    index2SideLineInfo_table[i] = *iter2;
                    if(iter2->balise_ID.size()>=2)//链接数量大于两个说明是入站应答器组
                    {
                        ret.push_back(LineType::SideLineIn);
                        status = LineType::SideLine;
                    }
                    else//否则说明是出站应答器组
                    {
                        ret.push_back(LineType::SideLineOut);
                        status = LineType::Normal;
                    }
                    break;
                }
            }
        }
        if(ret.size()!=i+1)//说明在上一个for里没有找到
        {
            ret.push_back(status);
        }
    }
    assert(ret.size()==Balises.size());
    //如果该应答器在侧线中，那么就需要更新SideLineInfo信息
    if(ret[index]!=LineType::Normal)
    {
        BaliseLocation::SideLineInfoIn = index2SideLineInfo_table[FindSideLineInNearby(ret,index)];
        BaliseLocation::SideLineInfoOut = index2SideLineInfo_table[FindSideLineOutNearby(ret,index)];
    }
    return ret;
}

/**
 * @brief BaliseLocation::FindSideLineInNearby 找到最近的属性为SideLineIn的应答器组
 * @param labels 一系列应答器组的属性
 * @param index 当前应答器组的位置
 * @return 最近的属性为SideLineIn的应答器组的位置，没有的话为-1
 */
int BaliseLocation::FindSideLineInNearby(const QList<LineType>& labels,int index)
{
    while(index>0)
    {
        if(labels[index]==LineType::SideLineIn)
            break;
        index--;
    }
    return index;
}

/**
 * @brief BaliseLocation::FindSideLineOutNearby 找到最近的属性为SideLineOut的应答器组
 * @param labels 一系列应答器组的属性
 * @param index 当前应答器组的位置
 * @return 最近的属性为SideLineOut的应答器组的位置，没有的话为-1
 */
int BaliseLocation::FindSideLineOutNearby(const QList<LineType>& labels,int index)
{
    while(index<labels.size())
    {
        if(labels[index]==LineType::SideLineOut)
            break;
        index++;
    }
    if(index==labels.size())
        return -1;
    return index;
}

AccessRode_MapType BaliseLocation::GetSideLineInfo(const QString BaliseGroup,const QString nextBaliseGroup)
{
    AccessRode_MapType ret;
    auto iter = DesignData::accessRodeMap.equal_range(BaliseGroup);
    if(iter.first == iter.second)//在进路表里没有找到以这个应答器组为起点的信息，那么延续之前的状态标识
    {
        return ret;
    }
    for(auto iter2=iter.first;iter2!=iter.second;iter2++)
    {
        if(!iter2->balise_ID.isEmpty())//应答器链接关系不为空的情况下
        {
            if(iter2->balise_ID[0]==nextBaliseGroup)
            {
                return *iter2;
            }
        }
    }
    return ret;
}

int findBrokenLength(int BaliseKM1, int BaliseKM2, Line_Dir dir)
{
	QString line_type = dir == UP ? "上" : "下";
	for (auto& line : DesignData::brokenLinkVec)
	{
		if (line.lineType.contains(line_type))
		{
			int point = DesignData::ConvertKmStr2Num(line.brokenLinkPosSta);
			if (BaliseKM1 < point && BaliseKM2 > point)
			{
				return line.brokenLinkType.contains("短") ? DesignData::ConvertKmStr2Num(line.brokenLinkShortLen) * -1 : DesignData::ConvertKmStr2Num(line.brokenLinkLongLen);
			}
		}
	}
	return 0;
}

//117-1-02-031
//117-1-01-114
QVector<QVector<QString>> BaliseLocation::Compare(const BalisePackCollector& BPK,QList<QString> RecordBaliseGroups)
{
    if (DesignData::baliseLocationUpMap.empty() || DesignData::baliseLocationDownMap.empty())//如果存在没有导入的应答器位置表，那么直接退出
        return {};
    //用于记录本应答器组编号的变量
    QString BaliseGroupID = RecordBaliseGroups.back();
    //按顺序获取本应答器链接的所有应答器组的编号
    QList<Record_Baliseposition> AllLinkedBalishGroup = LinkedBliseGroupID(BPK);
    if(AllLinkedBalishGroup.isEmpty())
        return {};
    const int ComparedGroupIndex = RecordBaliseGroups.size()-1;
    for(auto& arg:AllLinkedBalishGroup)
        RecordBaliseGroups.push_back(arg.ID);
    //获取应答器的报文内容
    if(RecordBaliseGroups.isEmpty())
        return {};
    QList<int> distance;
    for(int i=0;i<RecordBaliseGroups.size()-1;++i)
        distance.append(-1);
    for(int i=0;i<AllLinkedBalishGroup.size();++i)
        distance[i+ComparedGroupIndex] = AllLinkedBalishGroup[i].D_LINK;
    QPair<QList<QString>,QList<int>> BaliseInfo = {RecordBaliseGroups.mid(ComparedGroupIndex+1),distance.mid(ComparedGroupIndex)};

	QList<int> distance_err;
	for (int i = 0; i < distance.size(); ++i)
		distance_err.append(0);
    //获取数据表的记录内容
    for(int i=0;i<distance.size();++i)
    {
		int KM1 = DesignData::FindBaliseKmByID(RecordBaliseGroups[i + 1]);
		int KM2 = DesignData::FindBaliseKmByID(RecordBaliseGroups[i]);
        distance[i] = abs(KM1-KM2);
		distance_err[i] = findBrokenLength(std::min(KM1, KM2), std::max(KM1, KM2), g_LineDirection);
    }
    QList<LineType> labels = JudgeSideLineBaliseGroup(RecordBaliseGroups,ComparedGroupIndex);
    int startIndex = ComparedGroupIndex;
    if(labels[ComparedGroupIndex]!=LineType::Normal)
    {
        startIndex = FindSideLineInNearby(labels,ComparedGroupIndex);
        if(startIndex==-1)//错误处理
        {
            return {};
        }
    }
	//侧线替换
    for(int i=startIndex;i<RecordBaliseGroups.size();++i)
        if(labels[i]==LineType::SideLineIn||labels[i]==LineType::SideLineOut)
        {
            AccessRode_MapType am = GetSideLineInfo(RecordBaliseGroups[i],RecordBaliseGroups[i+1]);
            if(am.isEmpty())
                return {};
            assert(am.balise_ID.size()==am.balise_Diatance.size());
            for(int j=0;j<am.balise_ID.size();++j)
            {
                RecordBaliseGroups[i+j+1] = am.balise_ID[j];
                distance[i+j] = am.balise_Diatance[j];
				distance_err[i + j] = 0;
            }
        }
	QPair<QList<QString>, QList<int>> TableInfo = { RecordBaliseGroups.mid(ComparedGroupIndex + 1),distance.mid(ComparedGroupIndex) };
	labels = labels.mid(ComparedGroupIndex + 1);
	distance_err = distance_err.mid(ComparedGroupIndex);

    QVector<QVector<QString>> tempstring_collector_new;
    QVector<QString> tempstring_collector_item;
    tempstring_collector_new<<tempstring_collector_item;
    tempstring_collector_new[0].resize(3);
    tempstring_collector_new[0][0] = BaliseGroupID;
    tempstring_collector_new[0][1] = "Right";
    tempstring_collector_item<<""<<"应答器信息"<<"数据表信息";
    tempstring_collector_new.push_back(tempstring_collector_item);
    assert(TableInfo.first.size()==TableInfo.second.size());
    assert(BaliseInfo.first.size()==BaliseInfo.second.size());
    assert(BaliseInfo.first.size()==TableInfo.first.size());
    for(int i=0;i<TableInfo.first.size();++i)
    {
		//应答器组数据
        tempstring_collector_item.clear();
		tempstring_collector_item << "应答器组" << BaliseInfo.first[i]<< TableInfo.first[i];
		if (labels[i] != LineType::Normal)
			tempstring_collector_item[2] += "(侧线)";
        if(TableInfo.first[i]!=BaliseInfo.first[i])
        {
			tempstring_collector_item[1].append("W");
			tempstring_collector_item[2].append("W");
            tempstring_collector_new[0][1] = "Wrong";
        }
        tempstring_collector_new.push_back(tempstring_collector_item);
		//D_LINK数据
        tempstring_collector_item.clear();
		tempstring_collector_item << "D_LINK" << QString::number(BaliseInfo.second[i])<< QString::number(TableInfo.second[i]);
		if (distance_err[i] != 0)
		{
			if (distance_err[i] > 0)
				tempstring_collector_item[2] += "+" + QString::number(distance_err[i]);
			else
				tempstring_collector_item[2] += QString::number(distance_err[i]);
		}
        if(TableInfo.second[i]+distance_err[i]!=BaliseInfo.second[i])
        {
			tempstring_collector_item[1].append("W");
			tempstring_collector_item[2].append("W");
            tempstring_collector_new[0][1] = "Wrong";
        }
        tempstring_collector_new.push_back(tempstring_collector_item);
    }
    return tempstring_collector_new;

//    //计算长短链
//    for (int i = 0; i < DesignData::brokenLinkVec.size(); i++)//考虑长短链
//    {
//        if ((DesignData::brokenLinkVec[i].lineType.contains("上") && g_LineDirection == UP) || (DesignData::brokenLinkVec[i].lineType.contains("下") && g_LineDirection == DOWN))
//        {
//            if (now_position_excel > last_position_excel)//公里标增大方向
//            {
//                if (DesignData::ConvertKmStr2Num(DesignData::brokenLinkVec[i].brokenLinkPosSta) > std::min(now_position_excel, last_position_excel) && DesignData::ConvertKmStr2Num(DesignData::brokenLinkVec[i].brokenLinkPosSta) < std::max(now_position_excel, last_position_excel))//如果断链起始点在两次应答器位置的中间
//                {
//                    if (DesignData::brokenLinkVec[i].brokenLinkType.contains("短"))//短链加
//                        err += DesignData::ConvertKmStr2Num(DesignData::brokenLinkVec[i].brokenLinkShortLen);
//                    else if (DesignData::brokenLinkVec[i].brokenLinkType.contains("长"))//长链减
//                        err -= DesignData::ConvertKmStr2Num(DesignData::brokenLinkVec[i].brokenLinkLongLen);
//                }
//            }
//            else
//            {
//                if (DesignData::ConvertKmStr2Num(DesignData::brokenLinkVec[i].brokenLinkPosEnd) > std::min(now_position_excel, last_position_excel) && DesignData::ConvertKmStr2Num(DesignData::brokenLinkVec[i].brokenLinkPosSta) < std::max(now_position_excel, last_position_excel))//如果断链终止点在两次应答器位置的中间
//                {
//                    if (DesignData::brokenLinkVec[i].brokenLinkType.contains("短"))//短链-
//                        err += DesignData::ConvertKmStr2Num(DesignData::brokenLinkVec[i].brokenLinkShortLen);
//                    else if (DesignData::brokenLinkVec[i].brokenLinkType.contains("长"))//长链+
//                        err -= DesignData::ConvertKmStr2Num(DesignData::brokenLinkVec[i].brokenLinkLongLen);
//                }
//            }
//        }
//    }
    
//    //用于记录公里标差值和ATP自行走距离差值的变量定义
//    static qint32 last_position_excel = -1;
//    qint32 now_position_excel = -1;
//    qint32 diff_excel_position = -1;
//    //进行比对
//    {
//        //应答器编号统一成应答器组中第一个应答器的编号
//        QString first_Balish_ID = m_Balise_IDstr;
//        if (first_Balish_ID.size() != 12)
//            first_Balish_ID[first_Balish_ID.size()-1]= '1';
//        QString Km;
//        //在应答器位置表中寻找应答器位置
//        if (DesignData::baliseLocationUpMap.find(first_Balish_ID) != DesignData::baliseLocationUpMap.end())
//            Km = DesignData::baliseLocationUpMap[first_Balish_ID].baliseKm;
//        else if (DesignData::baliseLocationDownMap.find(first_Balish_ID) != DesignData::baliseLocationDownMap.end())
//            Km = DesignData::baliseLocationDownMap[first_Balish_ID].baliseKm;
//        else
//            qDebug() << "Balish not find in sheet data:" << first_Balish_ID;
//        now_position_excel = DesignData::ConvertKmStr2Num(Km);
//        //如果这是第一个比对的应答器组，那么无须比对，记录编号和公里标直接退出就好
//        if (last_balish_group_ID.isEmpty())
//        {
//            last_position_excel = now_position_excel;
//            last_balish_group_ID = BaliseGroupID;
//            return resultVec;
//        }
//        //表位置作差，如果本应答器位置找不到，差值置为-2，如果本应答器或者上一组应答器的公里标是负数，那么差值置为-3
//        diff_excel_position = Km.isEmpty() ? -2 : \
//            (now_position_excel > 0 && last_position_excel > 0) ? abs(now_position_excel - last_position_excel) : -3;
//        //计算长短链
//        for (int i = 0; i < DesignData::brokenLinkVec.size(); i++)//考虑长短链
//        {
//            if ((DesignData::brokenLinkVec[i].lineType.contains("上") && g_LineDirection == UP) || (DesignData::brokenLinkVec[i].lineType.contains("下") && g_LineDirection == DOWN))
//            {
//                if (now_position_excel > last_position_excel)//公里标增大方向
//                {
//                    if (DesignData::ConvertKmStr2Num(DesignData::brokenLinkVec[i].brokenLinkPosSta) > std::min(now_position_excel, last_position_excel) && DesignData::ConvertKmStr2Num(DesignData::brokenLinkVec[i].brokenLinkPosSta) < std::max(now_position_excel, last_position_excel))//如果断链起始点在两次应答器位置的中间
//                    {
//                        if (DesignData::brokenLinkVec[i].brokenLinkType.contains("短"))//短链加
//                            err += DesignData::ConvertKmStr2Num(DesignData::brokenLinkVec[i].brokenLinkShortLen);
//                        else if (DesignData::brokenLinkVec[i].brokenLinkType.contains("长"))//长链减
//                            err -= DesignData::ConvertKmStr2Num(DesignData::brokenLinkVec[i].brokenLinkLongLen);
//                    }
//                }
//                else
//                {
//                    if (DesignData::ConvertKmStr2Num(DesignData::brokenLinkVec[i].brokenLinkPosEnd) > std::min(now_position_excel, last_position_excel) && DesignData::ConvertKmStr2Num(DesignData::brokenLinkVec[i].brokenLinkPosSta) < std::max(now_position_excel, last_position_excel))//如果断链终止点在两次应答器位置的中间
//                    {
//                        if (DesignData::brokenLinkVec[i].brokenLinkType.contains("短"))//短链-
//                            err += DesignData::ConvertKmStr2Num(DesignData::brokenLinkVec[i].brokenLinkShortLen);
//                        else if (DesignData::brokenLinkVec[i].brokenLinkType.contains("长"))//长链+
//                            err -= DesignData::ConvertKmStr2Num(DesignData::brokenLinkVec[i].brokenLinkLongLen);
//                    }
//                }
//            }
//        }

//        //生成比对结果
//        QVector<QString> tempstring_collector;
//        //第一行为结果总览，包括应答器编号、属性、比对结果是否有误
//        tempstring_collector.push_back(m_Balise_IDstr+"("+ Balish_Category2Str[SideLineIndex]+")");//resultVec[0][0]是应答器标识号+应答器的属性
//        tempstring_collector.push_back(QString::number(m_udpDataOrder));//resultVec[0][1]是原始数据序号，DMS数据项中此数据无效
//        tempstring_collector.push_back("Wrong");//resultVec[0][2]是比对是否有错误，默认错误
//        resultVec.push_back(tempstring_collector);
//        //第二行为比对的具体内容
//        tempstring_collector.clear();
//        tempstring_collector.resize(4);
//        tempstring_collector[0] = "无记录";//上一组应答器记录的链接应答器组的ID
//        tempstring_collector[1] = "无D_LINK值";//D_LINK值
//        tempstring_collector[2] = QString::number(diff_excel_position);//应答器位置的差值（在工程数据表中）
//        tempstring_collector[3] = "无记录";//D_LINK允许误差
//        //具体比对的前提是有记录的链接关系
//        if (!ALGI.isEmpty())
//        {
//            //先默认比对结果是正确的，有任何一项有错误就把该变量置为Wrong
//            resultVec[0][2] = "Right";
//            Record_Baliseposition temp_front;
//            //取比对队列的第一个
//            temp_front = ALGI.front();
//            ALGI.pop_front();
//            tempstring_collector[0] = temp_front.ID;//上一组应答器记录的链接应答器组的ID
//            tempstring_collector[1] = QString::number(temp_front.D_LINK);//上一组应答器记录的D_LINK值
//            tempstring_collector[2] = QString::number(diff_excel_position);//应答器位置的差值（在工程数据表中）
//            tempstring_collector[3] = QString::number(temp_front.AllowErr) + "m" + "R";
//            //当应答器为侧线应答器的时候，进行侧线的特殊比对
//            if (SideLineIndex >= 0 && SideLineIndex <= 3)
//            {
//                switch (SideLineIndex)
//                {
//                case Balish_Category::JZ://进站应答器组直接按照正线逻辑比较
//                    break;
//                case Balish_Category::FCZ://反出站应答器组，按进站进路信息的第一个更新
//                    diff_excel_position = SideLineInfoIn.balise_Diatance.isEmpty() ? -4 : SideLineInfoIn.balise_Diatance[0];
//                    temp_front.ID = SideLineInfoIn.balise_ID.isEmpty() ? "无法提供" : SideLineInfoIn.balise_ID[0];
//                    break;
//                case Balish_Category::CZ://反出站应答器组，按进站进路信息的最后一个更新
//                    diff_excel_position = SideLineInfoIn.balise_Diatance.isEmpty() ? -5 : SideLineInfoIn.balise_Diatance.back();
//                    temp_front.ID = SideLineInfoIn.balise_ID.isEmpty() ? "无法提供" : SideLineInfoIn.balise_ID.back();
//                    break;
//                case Balish_Category::FJZ://出站应答器组，按出站进路信息更新
//                    diff_excel_position = SideLineInfoOut.balise_Diatance.isEmpty() ? -6 : SideLineInfoOut.balise_Diatance[0];
//                    temp_front.ID = SideLineInfoOut.balise_ID.isEmpty() ? "无法提供" : SideLineInfoOut.balise_ID[0];
//                    break;
//                default:
//                    diff_excel_position = -7;
//                    temp_front.ID = "侧线判定冲突";
//                    break;
//                }
//                if (diff_excel_position < 0)
//                    qDebug() << "stop";
//                tempstring_collector[0] = temp_front.ID;//上一组应答器记录的链接应答器组的ID
//                tempstring_collector[2] = QString::number(diff_excel_position);//应答器位置的差值（在工程数据表中）
//            }
//            //判断应答器组编号是否能和记录对应上
//            tempstring_collector[0].push_back(temp_front.ID == BaliseGroupID ? "R" : "W");
//            if(tempstring_collector[0][tempstring_collector[0].size()-1]=='W')
//                resultVec[0][2] = "Wrong";
//            //如果存在断链，那么相应的D_LINK值显示要加减对应的长短链
//            if (err)
//                tempstring_collector[1].append(err > 0 ? ("+" + QString::number(err)) : ("-" + QString::number(abs(err))));
//            //判断应答器中的D_LINK值与excel表中的差值是否在允许的误差范围内
//            if (abs(temp_front.D_LINK + err - diff_excel_position) <= temp_front.AllowErr)
//            {
//                tempstring_collector[1].push_back("R");
//                tempstring_collector[2].push_back("R");
//            }
//            else
//            {
//                tempstring_collector[1].push_back("W");
//                tempstring_collector[2].push_back("W");
//                resultVec[0][2] = "Wrong";
//            }
//        }
//        last_position_excel = now_position_excel;
//        last_balish_group_ID = BaliseGroupID;//更新记录的上一次进来的应答器组号
//        //比对结果的格式化
//        {
//            QVector<QVector<QString>> vec_1_new;
//            vec_1_new.push_back(QVector<QString>{"", "应答器内容", "数据表", "比对结果"});
//            QVector<QString> V_QSTR;
//            QChar rw = tempstring_collector[0][tempstring_collector[0].size()-1];
//            V_QSTR << "应答器组编号" << BaliseGroupID << tempstring_collector[0].mid(0, tempstring_collector[0].size() - 1) << (rw == 'R' ? "一致" : "不一致");
//            vec_1_new.push_back(V_QSTR);
//            V_QSTR.clear();
//            rw = tempstring_collector[1][tempstring_collector[1].size()-1];
//            V_QSTR << "D_LINK值" << tempstring_collector[1].mid(0, tempstring_collector[1].size() - 1) + "(允许误差:" + tempstring_collector[3].mid(0, tempstring_collector[3].size() - 1) + ")" << tempstring_collector[2].mid(0, tempstring_collector[2].size() - 1) + "(允许误差:" + tempstring_collector[3].mid(0, tempstring_collector[3].size() - 1) + ")" << (rw == 'R' ? "一致" : "不一致");
//            vec_1_new.push_back(V_QSTR);
//            resultVec.append(vec_1_new);
//        }
//    }
}
