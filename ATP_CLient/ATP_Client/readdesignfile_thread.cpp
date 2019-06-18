#include <readdesignfile_thread.h>
#include <QProcess>
#include "designdata.h"
#include <assert.h>
#include "basestructer.h"
#include <iostream>
#include <qmessagebox.h>
#include "Command.h"

//namespace std {
//
//	std::size_t hash(const QString & s) {
//		return qHash(s);
//	}
//}
QVector<SheetRecord> sRecordVec(0);
QMutex sheetMutex;

ReadFileThread::ReadFileThread(QObject *parent):QThread(parent)
{
	sRecordVec.clear();
}

/*************************************************************
 * @功能：将从Excel表中读到的QVariant类型数据转换为QList<QList<QVariant> >
 * @形参：QString fileName   FileType fileType
 * @返回值：void
 * @作者：hb
 * @修改日期：2017-5-21
*************************************************************/
void ReadFileThread::castVariant2ListListVariant(const QVariant &var, QList<QList<QVariant> > &res)
{
    QVariantList varRows = var.toList();
    if(varRows.isEmpty())
    {
        return;
    }
    const int rowCount = varRows.size();
    QVariantList rowData;
    for(int i=0;i<rowCount;++i)
    {
        rowData = varRows[i].toList();
        res.push_back(rowData);
    }
}

/**********************************************************
 * @功能：将数字转换为Excel表的列号
 * @形参：num 待转数字
 * @返回值：字母形式列号
 * @作者：hb
 * @修改日期：2017-5-24
**********************************************************/
QString ReadFileThread::num2Alpha(int num)
{
    char first;
    char last;
    QString firstStr;
    QString lastStr;
    int fre = num/26;
    if(fre == 0)
    {
        firstStr = "";
    }
    else
    {
        first = fre + 64;
        firstStr = first;
    }
    int yushu = num % 26;
    if(yushu == 0)
    {
        lastStr = "";
    }
    else
    {
        last = yushu + 64;
        lastStr = last;
    }
    return firstStr+lastStr;
}

QString ReadFileThread::processKm(QString kmStr)
{
    QString numberStr;
    for(int i = 0; i < kmStr.size(); i++)
    {
        if(kmStr[i] >= '0' && kmStr[i] <= '9')
        {
            numberStr.append(kmStr[i]);
        }
    }
    return numberStr;
}

/******************************************************************
 * @功能：打开fileName指定Excel表，根据表中内容填充到designdata中的Vector
 * @形参：QString fileName   FileType fileType
 * @返回值：bool true 成功 false 失败
 * @作者：hb
 * @修改日期：2017-5-21
*******************************************************************/
bool ReadFileThread::listHandler(QAxObject* excel, QString fileName, FileType fileType)
{
    bool returnFlag = true;
    QAxObject *workbooks = excel->querySubObject("WorkBooks");//获取工作簿集合
	if (workbooks == nullptr)
	{
		emit showResult("Excel表格打开失败");
		return false;
	}
    workbooks->dynamicCall("Open (const QString&)", fileName); //打开进路信息表
    QAxObject *workbook = excel->querySubObject("ActiveWorkBook");//获取活动工作簿
	if (workbook == nullptr)
	{
		emit showResult("Excel表格打开失败");
		return false;
	}
    QAxObject *worksheets = workbook->querySubObject("WorkSheets");//获取所有的工作表
    int sheetCount = worksheets->property("Count").toInt();  //获取工作表数目
    qDebug() <<"sheetCount"<<sheetCount;

	//判断字表个数是否符合规范
	switch (fileType)
	{
	case JINLU:
		if (sheetCount != 1)
		{
			emit showResult("进路信息表子表个数不为1");
			returnFlag = false;
			return returnFlag;
		}
		break;
	case CHEZHAN:
		if (sheetCount != 1)
		{
			emit showResult("车站信息表子表个数不为1");
			returnFlag = false;
			return returnFlag;
		}
		break;
	case PODU:
		if (sheetCount <= 0 || sheetCount > 4)
		{
			emit showResult("坡度信息表子表个数不符合规范");
			returnFlag = false;
			return returnFlag;
		}
		break;
	case GUOFENXIANG:
		if (sheetCount <= 0 || sheetCount > 2)
		{
			emit showResult("分项区信息表子表个数不符合规范");
			returnFlag = false;
			return returnFlag;
		}
		break;
	case SHUJU:
		if (sheetCount <= 0 || sheetCount > 4)
		{
			emit showResult("数据信息表子表个数不符合规范");
			returnFlag = false;
			return returnFlag;
		}
		break;
	case SUDU:
		if (sheetCount <= 0 || sheetCount > 4)
		{
			emit showResult("速度信息表子表个数不符合规范");
			returnFlag = false;
			return returnFlag;
		}
		break;
	case YINGDAQI:
		if (sheetCount <= 0 || sheetCount > 2)
		{
			emit showResult("应答器位置表子表个数不符合规范");
			returnFlag = false;
			return returnFlag;
		}
		break;
	case DUANLIAN:
		if (sheetCount != 1)
		{
			emit showResult("断链信息表子表个数不为1");
			returnFlag = false;
			return returnFlag;
		}
		break;
	case ZUOBIAOXI:
		if (sheetCount != 1)
		{
			emit showResult("坐标系信息表子表个数不为1");
			returnFlag = false;
			return returnFlag;
		}
		break;
	case LICHENG:
		if (sheetCount != 1)
		{
			emit showResult("里程信息表子表个数不为1");
			returnFlag = false;
			return returnFlag;
		}
		break;
	case ZHANTAI:
		if (sheetCount != 1)
		{
			emit showResult("站台侧信息表子表个数不为1");
			returnFlag = false;
			return returnFlag;
		}
		break;
	default:break;
	}

    for(int i = 1; i <= sheetCount; i++)
    {
        QAxObject *worksheet = workbook->querySubObject("Sheets(int)", i);//获取第i个工作表
        QString sheetName = worksheet->property("Name").toString();
        qDebug()<<sheetName;
        QAxObject *usedRange = worksheet->querySubObject("UsedRange");
        QAxObject *rows = usedRange->querySubObject("Rows");
        int rowCount = rows->property("Count").toInt();  //获取行数
        qDebug()<<"行数"<<rowCount;
        QAxObject *cols = usedRange->querySubObject("Columns");
        int colCount = cols->property("Count").toInt();  //获取列数
        qDebug()<<"列数"<<colCount;

        //确定表的第一列范围，从A1到“A终止行”
        QString str = QString::number(rowCount, 10);
        qDebug()<<"A" + str;
        QVariantList params;
        params << "A1" << "A" + str;

        //读取第一列
        QAxObject *col_1 = worksheet->querySubObject("Range(QVariant,QVariant)", params);
        QVariant col_1_data = col_1->dynamicCall("Value2()");
        //将数据转化为 QList<QList<QVariant>> 格式
        QList< QList<QVariant> > col_1_list;
        castVariant2ListListVariant(col_1_data, col_1_list);

        //确定起始行号和终止行号，以序号1开始至最终序号
        int startRow = 1;
        int endRow = 1;
        bool once = true;
        bool ok = false;
        qDebug()<<col_1_list;
        qDebug()<<col_1_list.count();
        for(int y = 0; y < col_1_list.count(); y++)
        {
           col_1_list.at(y).at(0).toString().toDouble(&ok); //判断得到的字符串能否转化为double
           if(ok && once)
           {
               qDebug()<<"y"<<y;
               startRow = y + 1; //excel表从1开始
               once = false;
           }
           if(!ok && !once)
           {
               endRow = y;
               break;
           }
           endRow = y + 1;
        }

        qDebug()<<"起始行"<<startRow;
        qDebug()<<"终止行"<<endRow;

        //确定表格数据内容读取范围 从“A起始行” 到“终止列终止行”
        params.clear();
        str = QString::number(startRow, 10);
        params << "A" + str;
        str = QString::number(endRow, 10);
        params << num2Alpha(colCount) + str;

        qDebug() << "数据范围" << params;

        //读取指定数据范围内表格内容到dataItemList
        QAxObject *data = worksheet->querySubObject("Range(QVariant,QVariant)", params);
        QVariant dataItem = data->dynamicCall("Value2()");
        QList< QList<QVariant> > dataItemList;
        castVariant2ListListVariant(dataItem, dataItemList);

        switch(fileType)
        {
            case JINLU:
                {
					AccessRode_Sheet temp_accessRode;
					temp_accessRode.sheet_name = fileName;
                    //遍历每行，填充结构体信息并放入Vector
					QString hold_str_balishID = QString("-");
					QString hold_str_balish = QString("/,");
					QString hold_str_speed = QString("/,");
					QString hold_str_pathway = QString("\\-,");
                    for(int k = 0; k <= endRow - startRow; k++)
                    {
                        AccessRode accseeRode;
                        accseeRode.ID = dataItemList.at(k).at(0).toString();
                        accseeRode.baliseID = strFilter(dataItemList.at(k).at(1).toString(), hold_str_balishID, FilterType::Num, false);
						if (accseeRode.baliseID.size() != 12 && accseeRode.baliseID.size() != 14)//应答器编号为组内编号时应该是严格的14个字符，为组编号时应该是严格的12字符
							accseeRode.baliseID.clear();
						accseeRode.routeID = dataItemList.at(k).at(2).toString();
                        accseeRode.route = dataItemList.at(k).at(3).toString();
                        accseeRode.routeType = dataItemList.at(k).at(4).toString();
                        accseeRode.annBeginName = dataItemList.at(k).at(5).toString();
                        accseeRode.annBeginCodeOrderMax = dataItemList.at(k).at(6).toString();
                        accseeRode.annunciatorEndName = dataItemList.at(k).at(7).toString();

						//考虑输入逗号时的中英文混输入情况
                        accseeRode.balise = dataItemList.at(k).at(8).toString();
						accseeRode.balise.replace("，", ",");
						accseeRode.balise = strFilter(accseeRode.balise, hold_str_balish, FilterType::Num, false);

						accseeRode.turnout = dataItemList.at(k).at(9).toString();

						//考虑输入逗号时的中英文混输入情况
                        accseeRode.speed = dataItemList.at(k).at(10).toString();
						accseeRode.speed.replace("，", ",");
						accseeRode.speed = strFilter(accseeRode.speed, hold_str_speed, FilterType::Num, false);

						//考虑输入逗号时的中英文混输入情况
                        accseeRode.pathWay = dataItemList.at(k).at(11).toString();
						accseeRode.pathWay.replace("，", ",");
						accseeRode.pathWay = strFilter(accseeRode.pathWay, hold_str_pathway, (FilterType)(FilterType::Num | FilterType::Chinese | FilterType::LowerLetter | FilterType::UpperLetter), false);
                        
						accseeRode.disasterPreLen = dataItemList.at(k).at(12).toString();
                        accseeRode.remark = dataItemList.at(k).at(13).toString();
						temp_accessRode.sheet.push_back(accseeRode);

                    }
					for (unsigned int i = 0; i < temp_accessRode.sheet.size(); i++)
						qDebug() << temp_accessRode.sheet[i].ID << " " << temp_accessRode.sheet[i].baliseID << " " << \
						temp_accessRode.sheet[i].balise << " " << temp_accessRode.sheet[i].speed << " " << temp_accessRode.sheet[i].pathWay;

					int order = 1;
					//进行进路标的出站信息补全
					for (unsigned int i = 0; i < temp_accessRode.sheet.size(); i++)
					{
						if (temp_accessRode.sheet[i].balise.size() < 3 && temp_accessRode.sheet[i].baliseID.size())//当该表格的内容的字符数小于3的时候，就认为是空表，因为该表内容可能为"-"或者没有数据，或者其他形式
						{
							QStringList list_str = temp_accessRode.sheet[i].baliseID.split("-");//获取其应答器组编号
							for (unsigned int j = 0; j < temp_accessRode.sheet.size(); j++)
							{
								//两个判定条件：1，本条的应答器组编号和遍历找到的目标条的链接应答器组编号相同。2，本条的终端信号机的名称与目标条的始端信号机的名称相符
								if (list_str[3] == temp_accessRode.sheet[j].balise.split("/")[0] && temp_accessRode.sheet[i].annunciatorEndName == temp_accessRode.sheet[j].annBeginName)
									temp_accessRode.sheet[i].balise = temp_accessRode.sheet[j].baliseID.split("-")[3] + "/" + temp_accessRode.sheet[j].balise.split(",")[0].split("/")[1];
							}
						}
					}
					//将数据格式进行规整化
					for (unsigned int i = 0; i < temp_accessRode.sheet.size(); i++)
					{
						AccessRode_MapType am;
						QStringList tempstrlist;
						if (temp_accessRode.sheet[i].balise.size())//不为空的时候对链接应答器组信息进行规整操作
						{
							tempstrlist.clear();
							tempstrlist = temp_accessRode.sheet[i].balise.split(",");
							QString balise_station_ID = temp_accessRode.sheet[i].baliseID;
							balise_station_ID.resize(8);//只保留大区小区车站编号，去掉应答器编号和可能存在的应答器组内标识号
							for (unsigned int i = 0; i < tempstrlist.size(); i++)
							{
								am.balise_ID.push_back(balise_station_ID + "-" + tempstrlist[i].split("/")[0]);
								am.balise_Diatance.push_back(tempstrlist[i].split("/")[1].toInt());
							}
							am.balise_ID.squeeze();
							am.balise_Diatance.squeeze();
						}
						if (temp_accessRode.sheet[i].speed.size())
						{
							tempstrlist.clear();
							tempstrlist = temp_accessRode.sheet[i].speed.split(",");
							for (unsigned int i = 0; i < tempstrlist.size(); i++)
							{
								am.speed.push_back(tempstrlist[i].split("/")[0].toInt());
								am.speed_Length.push_back(tempstrlist[i].split("/")[1].toInt());
							}
							am.speed.squeeze();
							am.speed_Length.squeeze();
						}
						if (temp_accessRode.sheet[i].pathWay.size())
						{
							tempstrlist.clear();
							QStringList pathwaylist;
							tempstrlist = temp_accessRode.sheet[i].pathWay.split(",");
							for (unsigned int i = 0; i < tempstrlist.size(); i++)
							{
								pathwaylist.clear();
								pathwaylist = tempstrlist[i].split("\\");
								if (pathwaylist.size() == 4)
								{
									am.pathWay_Length.push_back(pathwaylist[0].toInt());
									am.pathWay_Fre.push_back(pathwaylist[1]);
									am.pathWay_SignalType.push_back(pathwaylist[2]);
									am.pathWay_Name.push_back(pathwaylist[3]);
								}
							}
							am.pathWay_Length.squeeze();
							am.pathWay_Fre.squeeze();
							am.pathWay_SignalType.squeeze();
							am.pathWay_Name.squeeze();
						}
						if (temp_accessRode.sheet[i].annBeginName.size())
						{
							am.annBeginName = temp_accessRode.sheet[i].annBeginName;
						}
						if (temp_accessRode.sheet[i].annunciatorEndName.size())
						{
							am.annEndName = temp_accessRode.sheet[i].annunciatorEndName;
						}
						
						qDebug() << order++;
						for (int i = 0; i < am.balise_ID.size(); i++)
							qDebug() << am.balise_ID[i] << am.balise_Diatance[i];
						for (int i = 0; i < am.speed.size(); i++)
							qDebug() << am.speed[i] << am.speed_Length[i];
						for (int i = 0; i < am.pathWay_Fre.size(); i++)
							qDebug() << am.pathWay_Length[i] << am.pathWay_Fre[i]<< am.pathWay_SignalType[i]<< am.pathWay_Name[i];
						qDebug() << "_____________________________________________________________________________";

						DesignData::accessRodeMap.insert(temp_accessRode.sheet[i].baliseID.mid(0,12), am);
					}
                    break;
                }
            case CHEZHAN:
                {

                    for(int k = 0; k <= endRow - startRow; k++)
                    {
                        Station station;
                        station.ID = dataItemList.at(k).at(0).toString();
                        station.stationName = dataItemList.at(k).at(1).toString();
                        station.regionID = dataItemList.at(k).at(2).toString();
                        station.subareaID = dataItemList.at(k).at(3).toString();
                        station.stationID = dataItemList.at(k).at(4).toString();
                        DesignData::stationVec.push_back(station);
                    }
                    break;
                }
            case PODU:
                {

                    for(int k = 0; k <= endRow - startRow; k++)
                    {
                        Grade grade;
                        grade.ID = dataItemList.at(k).at(0).toString();
                        grade.grade = dataItemList.at(k).at(1).toString();
                        grade.len = dataItemList.at(k).at(2).toString();
                        grade.endKm = processKm(dataItemList.at(k).at(3).toString());
                        grade.remark = dataItemList.at(k).at(4).toString();

                        if(sheetName == "下行" || sheetName == "下行线")
                        {
							DesignData::gradeDownProVec.push_back(grade);
                        }
                        else if(sheetName == "上行" || sheetName == "上行线")
                        {
							DesignData::gradeUpProVec.push_back(grade);
                        }
                        else if(sheetName == "下行正向" || sheetName == "下行线正向")
                        {
                            DesignData::gradeDownProVec.push_back(grade);
                        }
                        else if(sheetName == "下行反向" || sheetName == "下行线反向")
                        {
                            DesignData::gradeDownBackVec.push_back(grade);
                        }
                        else if(sheetName == "上行正向" || sheetName == "上行线正向")
                        {
                            DesignData::gradeUpProVec.push_back(grade);
                        }
                        else if(sheetName == "上行反向" || sheetName == "上行线反向")
                        {
                            DesignData::gradeUpBackVec.push_back(grade);
                        }
                        else
                        {
                            emit (showResult("在坡度表中未检测到符合命名规范的子表"));
                            returnFlag = false;
							break;
                        }

                    }
                    break;
                }
            case GUOFENXIANG:
                {

                    for(int k = 0; k <= endRow - startRow; k++)
                    {
                        NeutralSection neutralSection;
                        neutralSection.ID = dataItemList.at(k).at(0).toString();
                        neutralSection.beganKm = processKm(dataItemList.at(k).at(1).toString());
                        neutralSection.endKm = processKm(dataItemList.at(k).at(2).toString());
                        neutralSection.len = dataItemList.at(k).at(3).toString();
                        neutralSection.remark = dataItemList.at(k).at(4).toString();
                        if(sheetName.contains("下行"))
                        {
                            DesignData::neutralSectionDownVec.push_back(neutralSection);
                        }
                        else if(sheetName.contains("上行"))
                        {
                            DesignData::neutralSectionUpVec.push_back(neutralSection);
                        }
                        else
                        {
                            emit showResult("在分相区表中未检测到符合命名规范的子表");
                            returnFlag = false;
							break;
                        }

                    }
                    break;
                }
            case SHUJU:
                {

                   for(int k = 0; k <= endRow - startRow; k++)
                    {
                        PathWayData pathWayData;
                        pathWayData.ID = dataItemList.at(k).at(0).toString();
                        pathWayData.stationName = dataItemList.at(k).at(1).toString();
                        pathWayData.signalSpotName = dataItemList.at(k).at(2).toString();
                        pathWayData.signalSpotKm = dataItemList.at(k).at(3).toString();
                        pathWayData.signalSpotType = dataItemList.at(k).at(4).toString();
                        pathWayData.insulationJointType = dataItemList.at(k).at(5).toString();
                        pathWayData.pathWayName = dataItemList.at(k).at(6).toString();
                        pathWayData.pathWayFre= dataItemList.at(k).at(7).toString();
                        pathWayData.pathWayLen = dataItemList.at(k).at(8).toString();
                        pathWayData.pathWayCof = dataItemList.at(k).at(9).toString();
                        pathWayData.remark = dataItemList.at(k).at(10).toString();
                        if(sheetName == "下行正向" || sheetName == "下行线正向")
                        {
                            DesignData::pathWayDataDownProVec.push_back(pathWayData);
                        }
                        else if(sheetName == "上行正向" || sheetName == "上行线正向")
                        {
                            DesignData::pathWayDataUpProVec.push_back(pathWayData);
                        }
                        else if(sheetName == "下行反向" || sheetName == "下行线反向")
                        {
                            DesignData::pathWayDataDownBackVec.push_back(pathWayData);
                        }
                        else if(sheetName == "上行反向" || sheetName == "上行线反向")
                        {
                            DesignData::pathWayDataUpBackVec.push_back(pathWayData);
                        }
                        else
                        {
                            emit showResult("在线路数据表中未检测到符合命名规范的子表");
                            returnFlag = false;
							break;
                        }

                    }
                    break;
                 }
            case SUDU:
                {

                    for(int k = 0; k <= endRow - startRow; k++)
                    {
                        PathWaySpeed pathWaySpeed;
                        pathWaySpeed.ID = dataItemList.at(k).at(0).toString();
                        pathWaySpeed.speed = dataItemList.at(k).at(1).toString();
                        pathWaySpeed.len = dataItemList.at(k).at(2).toString();
                        pathWaySpeed.endKm = processKm(dataItemList.at(k).at(3).toString());
                        pathWaySpeed.remark = dataItemList.at(k).at(4).toString();
                        if(sheetName == "下行" || sheetName == "下行正向" || sheetName == "下行线" || sheetName == "下行线正向")
                        {
                            DesignData::pathWaySpeedDownVec.push_back(pathWaySpeed);
                        }
                        else if(sheetName == "上行" || sheetName == "上行正向" || sheetName == "上行线" || sheetName == "上行线正向")
                        {
                            DesignData::pathWaySpeedUpVec.push_back(pathWaySpeed);
                        }
                        else if(sheetName == "下行反向" || sheetName == "下行线反向")
                        {
                            DesignData::pathWaySpeedDownBackVec.push_back(pathWaySpeed);
                        }
                        else if(sheetName == "上行反向" || sheetName == "上行线反向")
                        {
                            DesignData::pathWaySpeedUpBackVec.push_back(pathWaySpeed);
                        }
                        else
                        {
                            emit showResult("在线路速度表中未检测到符合命名规范的子表");
                            returnFlag = false;
							break;
                        }

                    }
                    break;
                }
            case YINGDAQI:
                {

                    for(int k = 0; k <= endRow - startRow; k++)
                    {
                        BaliseLocation baliseLocation;
                        baliseLocation.ID = dataItemList.at(k).at(0).toString();
                        baliseLocation.baliseName = dataItemList.at(k).at(1).toString();
                        baliseLocation.baliseID = dataItemList.at(k).at(2).toString();
                        baliseLocation.baliseKm = processKm(dataItemList.at(k).at(3).toString());
                        baliseLocation.baliseType = dataItemList.at(k).at(4).toString();
                        baliseLocation.baliseUse = dataItemList.at(k).at(5).toString();
                        baliseLocation.remark_1 = dataItemList.at(k).at(6).toString();
                        baliseLocation.remark_2 = dataItemList.at(k).at(7).toString();
						if (baliseLocation.baliseID == "075-5-13-011-1")
							qDebug() << "stop";
                        if(sheetName.contains("下行"))
                        {
							DesignData::baliseLocationDownMap.insert(baliseLocation.baliseID,baliseLocation);
							if(DesignData::balishUseMap.find(baliseLocation.baliseID.mid(0, 12))== DesignData::balishUseMap.end())
								DesignData::balishUseMap.insert(baliseLocation.baliseID.mid(0,12), baliseLocation.baliseUse);
                        }
                        else if(sheetName.contains("上行"))
                        {
							DesignData::baliseLocationUpMap.insert(baliseLocation.baliseID, baliseLocation);
							if (DesignData::balishUseMap.find(baliseLocation.baliseID.mid(0, 12)) == DesignData::balishUseMap.end())
								DesignData::balishUseMap.insert(baliseLocation.baliseID.mid(0, 12), baliseLocation.baliseUse);
                        }
                        else
                        {
                            emit showResult("在应答器位置表中未检测到符合命名规范的子表");
                            returnFlag = false;
							break;
                        }

                    }
                    break;
                }
            case DUANLIAN:
                {

                    for(int k = 0; k <= endRow - startRow; k++)
                    {
                        BrokenLink brokenLink;
                        brokenLink.lineName = dataItemList.at(k).at(0).toString();
                        brokenLink.lineType = dataItemList.at(k).at(1).toString();
                        brokenLink.brokenLinkType = dataItemList.at(k).at(2).toString();
                        brokenLink.brokenLinkPosSta = dataItemList.at(k).at(3).toString();
                        brokenLink.brokenLinkPosEnd = dataItemList.at(k).at(4).toString();
                        brokenLink.brokenLinkLongLen = dataItemList.at(k).at(5).toString();
                        brokenLink.brokenLinkShortLen = dataItemList.at(k).at(6).toString();
                        DesignData::brokenLinkVec.append(brokenLink);
                    }
                    break;
                }
            case ZHANTAI:
			{

				for (int k = 0; k <= endRow - startRow; k++)
				{
					StationSide stationSide;
					stationSide.ID = dataItemList.at(k).at(0).toString();
					stationSide.stationName = dataItemList.at(k).at(1).toString();
					stationSide.trackName = dataItemList.at(k).at(2).toString();
					stationSide.baliseName = dataItemList.at(k).at(3).toString();
					stationSide.stationSide = dataItemList.at(k).at(4).toString();
					DesignData::stationSideVec.append(stationSide);
				}
				break;
			}
		default:
			break;
		}
    }
    return returnFlag;
}

void ReadFileThread::run()
{
	if (filePath.count() != 0)
	{
		/*----------------------创建EXCEL----------------------------------------------*/
		//在子线程中使用COM组件需手动初始化
		HRESULT r = OleInitialize(0);
		if (r != S_OK && r != S_FALSE) {
			qWarning("Qt: Could not initialize OLE (error %x)", (unsigned int)r);
		}

		QAxObject *excel = NULL;
		excel = new QAxObject("Excel.Application");
		if (excel == NULL)
		{
			qDebug() << "打开EXCEL失败";
			emit showResult("打开EXCEL失败，请检查EXCEL是否安装或激活！");
			emit showResult("\n");
			return;
		}
		excel->setProperty("Visible", false); //不显示窗体
		excel->setProperty("DisplayAlerts", false);//不显示提示信息
/*--------------------------------------------------------------------------------*/

		//开始导入表格
		int num = 0;
		int step = 100 / filePath.count();
		QStringList jinluList = filePath.filter("进路");
		qDebug() << jinluList;
		if (!jinluList.isEmpty())
		{
			DesignData::clearAccessRodeVec();
			for (int i = 0; i < jinluList.size(); i++)
			{
				emit showResult("正在导入进路信息表...("+ jinluList.at(i)+")");
				if (listHandler(excel, jinluList.at(i), FileType::JINLU))
				{
					num += step;
					emit showProgressBar(num);
					emit showResult("进路信息表导入成功");
					emit addNumSignal(FileType::JINLU);
				}
				else
				{
					emit showResult("进路信息表导入失败");
				}
			}
		}
		else
		{
			emit showResult("进路信息表导入失败");
		}
		emit showResult("\n");

		QStringList chezhanList = filePath.filter("车站");
		qDebug() << chezhanList;
		if (!chezhanList.isEmpty() && chezhanList.count() == 1)
		{
			DesignData::clearStationVec();
			emit showResult("正在导入站台信息表...");
			if (listHandler(excel, chezhanList.at(0), FileType::CHEZHAN))
			{
				num += step;
				emit showProgressBar(num);
				emit showResult("车站信息表导入成功");
				emit addNumSignal(FileType::CHEZHAN);
			}
			else
			{
				emit showResult("车站信息表导入失败");
			}
		}
		else
		{
			emit showResult("未检测到车站信息表或表名重复");
			emit showResult("车站信息表导入失败");
		}
		emit showResult("\n");

		QStringList poduList = filePath.filter("坡度");
		qDebug() << poduList;
		if (!poduList.isEmpty() && poduList.count() == 1)
		{
			DesignData::clearGradeVec();
			emit showResult("正在导入线路坡度表...");
			if (listHandler(excel, poduList.at(0), FileType::PODU))
			{
				num += step;
				emit showProgressBar(num);
				emit showResult("线路坡度表导入成功");
				emit addNumSignal(FileType::PODU);
			}
			else
			{
				emit showResult("线路坡度表导入失败");
			}
		}
		else
		{
			emit showResult("未检测到线路坡度表或表名重复");
			emit showResult("线路坡度表导入失败");
		}
		emit showResult("\n");

		QStringList guofenxiangList = filePath.filter("分相");
		qDebug() << guofenxiangList;
		if (!guofenxiangList.isEmpty() && guofenxiangList.count() == 1)
		{
			DesignData::clearNeutralSectionVec();
			emit showResult("正在导入分相区信息表...");
			if (listHandler(excel, guofenxiangList.at(0), FileType::GUOFENXIANG))
			{
				num += step;
				emit showProgressBar(num);
				emit showResult("分相区信息表导入成功");
				emit addNumSignal(FileType::GUOFENXIANG);
			}
			else
			{
				emit showResult("分相区信息表导入失败");
			}
		}
		else
		{
			emit showResult("未检测到过分相信息表表或表名重复");
			emit showResult("分相区信息表导入失败");
		}
		emit showResult("\n");

		QStringList shujuList = filePath.filter("线路数据");
		qDebug() << shujuList;
		if (!shujuList.isEmpty() && shujuList.count() == 1)
		{
			DesignData::clearPathWayDataVec();
			emit showResult("正在导入线路数据表...");
			if (listHandler(excel, shujuList.at(0), FileType::SHUJU))
			{
				num += step;
				emit showProgressBar(num);
				emit showResult("线路数据表导入成功");
				emit addNumSignal(FileType::SHUJU);
			}
			else
			{
				emit showResult("线路数据表导入失败");
			}
		}
		else
		{
			emit showResult("未检测到线路数据表或表名重复");
			emit showResult("线路数据表导入失败");
		}
		emit showResult("\n");

		QStringList suduList = filePath.filter("速度");
		qDebug() << suduList;
		if (!suduList.isEmpty() && suduList.count() == 1)
		{
			DesignData::clearPathWaySpeedVec();
			emit showResult("正在导入线路速度表...");
			if (listHandler(excel, suduList.at(0), FileType::SUDU))
			{
				num += step;
				emit showProgressBar(num);
				emit showResult("线路速度表导入成功");
				emit addNumSignal(FileType::SUDU);
			}
			else
			{
				emit showResult("线路速度表导入失败");
			}
		}
		else
		{
			emit showResult("未检测到线路速度表或表名重复");
			emit showResult("线路速度表导入失败");
		}
		emit showResult("\n");

		QStringList yingdaqiList = filePath.filter("应答器位置");
		qDebug() << yingdaqiList;
		if (!yingdaqiList.isEmpty() && yingdaqiList.count() == 1)
		{
			DesignData::clearBaliseLocationVec();
			emit showResult("正在导入应答器位置表...");
			if (listHandler(excel, yingdaqiList.at(0), FileType::YINGDAQI))
			{
				num += step;
				emit showProgressBar(num);
				emit showResult("应答器位置表导入成功");
				emit addNumSignal(FileType::YINGDAQI);
			}
			else
			{
				emit showResult("应答器位置表导入失败");
			}
		}
		else
		{
			emit showResult("未检测到应答器位置表或表名重复");
			emit showResult("应答器位置表导入失败");
		}
		emit showResult("\n");

		QStringList duanLianList = filePath.filter("断链");
		qDebug() << duanLianList;
		if (!duanLianList.isEmpty() && duanLianList.count() == 1)
		{
			DesignData::clearBrokenLinkVec();
			emit showResult("正在导入里程断链明细表...");
			if (listHandler(excel, duanLianList.at(0), FileType::DUANLIAN))
			{
				num += step;
				emit showProgressBar(num);
				emit showResult("里程断链明细表导入成功");
				emit addNumSignal(FileType::DUANLIAN);
			}
			else
			{
				emit showResult("里程断链明细表导入失败");
			}
		}
		else
		{
			emit showResult("未检测到里程断链明细表或表名重复");
			emit showResult("里程断链明细表导入失败");
		}
		emit showResult("\n");

		QStringList zhanTaiCeList = filePath.filter("站台");
		qDebug() << zhanTaiCeList;
		if (!zhanTaiCeList.isEmpty() && zhanTaiCeList.count() == 1)
		{
			DesignData::clearStationSideVec();
			emit showResult("正在导入站台侧信息表...");
			if (listHandler(excel, zhanTaiCeList.at(0), FileType::ZHANTAI))
			{
				num += step;
				emit showProgressBar(num);
				emit showResult("站台侧信息表导入成功");
				emit addNumSignal(FileType::ZHANTAI);
			}
			else
			{
				emit showResult("站台侧信息表导入失败");
			}
		}
		else
		{
			emit showResult("未检测到站台侧信息表或表名重复");
			emit showResult("站台侧信息表导入失败");
		}
		emit showResult("\n");

		emit showProgressBar(100);
		emit showResult("表格导入完毕");

		QByteArray QBA;
		QDataStream QDS(&QBA,QIODevice::WriteOnly);
		QDS << DesignData::accessRodeMap << DesignData::stationVec << DesignData::gradeDownProVec << DesignData::gradeUpProVec\
			<< DesignData::gradeDownBackVec << DesignData::gradeUpBackVec << DesignData::neutralSectionUpVec << DesignData::neutralSectionDownVec\
			<< DesignData::pathWayDataDownProVec << DesignData::pathWayDataUpProVec << DesignData::pathWayDataDownBackVec << DesignData::pathWayDataUpBackVec\
			<< DesignData::pathWaySpeedDownVec << DesignData::pathWaySpeedUpVec << DesignData::pathWaySpeedDownBackVec << DesignData::pathWaySpeedUpBackVec\
			<< DesignData::baliseLocationUpMap << DesignData::baliseLocationDownMap << DesignData::balishUseMap << DesignData::brokenLinkVec << DesignData::stationSideVec;
		qDebug() << QBA.size();
		QFile dataSheet(QApplication::applicationDirPath() + '/' + "sheet.dat");
		if (dataSheet.open(QIODevice::WriteOnly))
		{
			dataSheet.write(QBA);
			dataSheet.close();
		}
		tcpCommandClient->send(Combine_Command_Data(TcpHead(CMD_FROM::CLIENT, CMD_TYPE::CONTROL, CMD_NAME::DATA_SHEET), QBA));

		emit showResult("数据已发出");

		/*-------------释放EXCEL------------------------------------------*/
		excel->dynamicCall("Quit(void)");  //退出
		delete excel;
		excel = NULL;
		OleUninitialize();
		/*----------------------------------------------------------------*/
	}
	else
	{
		emit showResult("未选择任何表格！");
	}
	//this->deleteLater();
    emit workfinished_SIGNAL();
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
QString ReadFileThread::strFilter(QString input, QString filter, FilterType filtertype, bool flag)
{
	QString ret;
	for (unsigned int i = 0; i < input.size(); i++)
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
		if (ch == 0)
		{
			for (unsigned int j = 0; j < filter.size(); j++)
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
			if (ch == 0)
				ret.push_back(input[i]);
		}
		else//保留
		{
			if (ch != 0)
				ret.push_back(input[i]);
		}
	}
	return ret;
}

