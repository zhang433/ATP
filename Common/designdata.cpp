#include "designdata.h"
#include <qmap.h>
#include <QDebug>


//********静态变量初始化*********************************
QMultiMap<QString, AccessRode_MapType> DesignData::accessRodeMap;

QVector<Station> DesignData::stationVec;

QVector<Grade> DesignData::gradeDownProVec;
QVector<Grade> DesignData::gradeUpProVec;
QVector<Grade> DesignData::gradeDownBackVec;
QVector<Grade> DesignData::gradeUpBackVec;

QVector<NeutralSection> DesignData::neutralSectionUpVec;   //分相区表-上行线
QVector<NeutralSection> DesignData::neutralSectionDownVec; //分相区表-下行线

QVector<PathWayData> DesignData::pathWayDataDownProVec;    //线路数据表-下行正向
QVector<PathWayData> DesignData::pathWayDataUpProVec;      //线路数据表-上行正向
QVector<PathWayData> DesignData::pathWayDataDownBackVec;   //线路数据表-下行反向
QVector<PathWayData> DesignData::pathWayDataUpBackVec;     //线路数据表-上行反向

QVector<PathWaySpeed> DesignData::pathWaySpeedDownVec;     //线路速度表-下行
QVector<PathWaySpeed> DesignData::pathWaySpeedUpVec;       //线路速度表-上行
QVector<PathWaySpeed> DesignData::pathWaySpeedDownBackVec; //线路速度表-下行反向
QVector<PathWaySpeed> DesignData::pathWaySpeedUpBackVec;   //线路速度表-上行反向

QMap<QString, BalisePosition> DesignData::baliseLocationUpMap;//应答器位置表-上行
QMap<QString, BalisePosition> DesignData::baliseLocationDownMap; //应答器位置表-下行
QMap<QString, QString> DesignData::balishUseMap; //应答器用途映射

QVector<BrokenLink> DesignData::brokenLinkVec;             //里程断链明细表

QVector<StationSide> DesignData::stationSideVec;           //站台侧信息表

void DesignData::clearAll()
{
	accessRodeMap.clear();
    stationVec.clear();

    gradeDownProVec.clear();
    gradeUpProVec.clear();
    gradeDownBackVec.clear();
    gradeUpBackVec.clear();

    neutralSectionUpVec.clear();   //分相区表-上行线
    neutralSectionDownVec.clear();

    pathWayDataDownProVec.clear();    //线路数据表-下行正向
    pathWayDataUpProVec.clear();      //线路数据表-上行正向
    pathWayDataDownBackVec.clear();   //线路数据表-下行反向
    pathWayDataUpBackVec.clear();     //线路数据表-上行反向

    pathWaySpeedDownVec.clear();     //线路速度表-下行
    pathWaySpeedUpVec.clear();       //线路速度表-上行
    pathWaySpeedDownBackVec.clear(); //线路速度表-下行反向
    pathWaySpeedUpBackVec.clear();   //线路速度表-上行反向

    brokenLinkVec.clear();             //里程断链明细表

    stationSideVec.clear();           //站台侧信息表
}

/*************************************************************************
*  函数名称：ConvertKmStr2Num
*  功能说明：用于将excel表格导入的公里标从字符串转换为数字形式
*  参数说明：读入的公里标字符串
*  函数返回：>=0时表示转换后的公里表的数值
*          <0时表示转换失败，公里表字符串存在问题
*  修改时间：2017-7-30
*  备   注：示例：假如输入的字符串是"K201+300","YK201+300","YK2W0%1&+300"，"201300",返回值均为int类型的201300
*************************************************************************/
int DesignData::ConvertKmStr2Num(QString input)
{
    if(input.indexOf("+")==-1)//如果没有+号，表明传入的可能是一个数字的string格式
    {
        bool ok=false;
        unsigned int temp_toint = input.toInt(&ok);
        if(ok)
            return temp_toint;
        else
            return -1;//转换失败
    }
    else
    {
        QStringList temp_split = input.split("+");//以+号为标志分割字符串
        if(temp_split.size()!=2)//分割后的字符串应该有2个部分，否则非既定格式，返回错误值
            return -1;//转换失败
        QString tempstr = temp_split.at(0);//第一个分割部分包含非数字字符，如K201
        QString str_number;
        for(auto it = tempstr.begin();it!=tempstr.end();it++)//将第一个分割部分所有数字字符筛选出来
        {
            if((*it)>='0'&&(*it)<='9')
                str_number.push_back(*it);
        }
        bool ok=false;
        unsigned int tempUint = str_number.toInt(&ok);
        if(!ok)
            return -1;//转换失败
        str_number = temp_split.at(1);//第二部分应该只包含数字字符项
        tempUint*=1000;
        tempUint+=str_number.toInt(&ok);
        if(!ok)
            return -1;
        else
            return tempUint;
    }
}

//*********************************************************************************************
//功能：提取长短链信息
//形参：包含长短链信息的字符串
//返回值：提取到的长短链值
//使用方法：静态方法，直接调用
//备注：
//修改日期：
//*********************************************************************************************
int DesignData::ConvertLinkMark2Num(const QString linStr)
{
    int index = linStr.indexOf("链");
    if(index == -1)
    {
        return -1;
    }
    else
    {
        QString tempStr;
        for(int i = index; i < linStr.size(); i++)
        {
            if(linStr[i] >= '0' && linStr[i] <= '9')
            {
                tempStr.append(linStr[i]);
            }
        }
        return tempStr.toInt();
    }
}

void DesignData::clearAccessRodeVec()
{
	accessRodeMap.clear();
}

void DesignData::clearStationVec()
{
    stationVec.clear();
}

void DesignData::clearGradeVec()
{
    gradeDownBackVec.clear();
    gradeDownProVec.clear();
    gradeUpBackVec.clear();
    gradeUpProVec.clear();
}

void DesignData::clearNeutralSectionVec()
{
    neutralSectionDownVec.clear();
    neutralSectionUpVec.clear();
}

void DesignData::clearPathWayDataVec()
{
    pathWayDataDownBackVec.clear();
    pathWayDataDownProVec.clear();
    pathWayDataUpBackVec.clear();
    pathWayDataUpProVec.clear();
}

void DesignData::clearPathWaySpeedVec()
{
    pathWaySpeedDownBackVec.clear();
    pathWaySpeedDownVec.clear();
    pathWaySpeedUpBackVec.clear();
    pathWaySpeedUpVec.clear();
}

void DesignData::clearBaliseLocationVec()
{
	baliseLocationUpMap.clear();
	baliseLocationDownMap.clear();
}

void DesignData::clearBrokenLinkVec()
{
    brokenLinkVec.clear();
}

void DesignData::clearStationSideVec()
{
    stationSideVec.clear();
}

/**
 * @brief DesignData::FindBaliseKmByID 返回应答器组第一个应答器公里标
 * @param baliseID 应答器编号，或者应答器组的编号
 * @return 应答器公里标
 */
int DesignData::FindBaliseKmByID(QString baliseID)
{
    if(baliseID.size()==12)//应答器组编号
    {
        if (baliseLocationUpMap.find(baliseID) != baliseLocationUpMap.end())
            return DesignData::ConvertKmStr2Num(baliseLocationUpMap[baliseID].baliseKm);
        else if (baliseLocationDownMap.find(baliseID) != baliseLocationDownMap.end())
            return DesignData::ConvertKmStr2Num(baliseLocationDownMap[baliseID].baliseKm);
        else
            baliseID+="-1";
    }
    else
        baliseID[baliseID.size()-1] = '1';
	if (baliseLocationUpMap.find(baliseID) != baliseLocationUpMap.end())
		return DesignData::ConvertKmStr2Num(baliseLocationUpMap[baliseID].baliseKm);
	else if (baliseLocationDownMap.find(baliseID) != baliseLocationDownMap.end())
		return DesignData::ConvertKmStr2Num(baliseLocationDownMap[baliseID].baliseKm);
    return -1;
}

/**************************************************
* @功能：返回应答器组第一个应答器备注信息
* @形参：baliseID - 应答器组中的任一一个应答器编号
* @返回值：应答器公里标
**************************************************/
QString DesignData::FindBaliseRemarkByID(QString baliseID)
{
	QString tempString = "";
	QStringList strList = baliseID.split("-");
	if (4 == strList.size())
	{
		tempString = baliseID;
	}
	if (5 == strList.size())
	{
		for (int i = 0; i < strList.size() - 1; i++)
		{
			tempString += strList[i] + "-";
		}
		tempString += "1";
	}
	if (baliseLocationUpMap.find(tempString) != baliseLocationUpMap.end())
	{
        return baliseLocationUpMap[tempString].remark_2;
	}
	else if (baliseLocationDownMap.find(tempString) != baliseLocationDownMap.end())
	{
        return baliseLocationDownMap[tempString].remark_2;
	}
	return "";
}
