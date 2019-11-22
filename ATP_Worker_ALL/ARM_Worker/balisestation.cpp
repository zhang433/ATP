#include "balisestation.h"
#include "basestructer.h"
#include "designdata.h"
#include "balisestation.h"
#include "self_define.h"
#include "analyze_resmessage.h"
#include <QStack>
#include <QString>
#include <QDebug>
extern Train_Dir g_TrainDirection;//列车运行方向
extern Line_Dir g_LineDirection;//列车运行线别

BaliseStation::BaliseStation()
{

}

/**
 * @brief BaliseStation::GetStationNameOfBliaseGroup 找到应答器组的所属车站名称
 * @param Balise 应答器(组)编号
 * @return 该应答器组所属的车站名称
 */
QString BaliseStation::GetStationNameOfBliaseGroup(const QString& Balise)
{
    auto sp = Balise.split("-");
    if(sp.size()>=3)
        for(auto& line:DesignData::stationVec)
        {
            if(line.regionID == sp[0] && line.subareaID == sp[1] && line.stationID == sp[2])
            {
                return line.stationName;
            }
        }
    return "";
}

QVector<QVector<QString>> BaliseStation::Compare(const BalisePackCollector& BPK,QString& BaliseID)//比对车站名，站台侧信息
{
    static QString RecordStationName,RecordBaliseID;
    static QString Cached_StationName;
    auto compare_station_name = [](QString& Cached_StationName,QString& BaliseID,QString& RecordBaliseID,QString& RecordStationName)->QVector<QVector<QString>>{
        if(Cached_StationName.isEmpty())
            return {};
        QString now_StationName = GetStationNameOfBliaseGroup(BaliseID);
        if(now_StationName.isEmpty())
            return {{"Can't find Station name of ["+BaliseID+"]","Wrong"}};

        QVector<QVector<QString>> tempstring_collector_new;
        QVector<QString> tempstring_collector_item;
        tempstring_collector_new.push_back(tempstring_collector_item);
        tempstring_collector_new[0].resize(2);
        tempstring_collector_new[0][0] = BaliseID;
        tempstring_collector_new[0][1] = "Right";
        tempstring_collector_item<<""<<"应答器内容"<<"数据表内容";
        tempstring_collector_new.push_back(tempstring_collector_item);
        tempstring_collector_item.clear();
        if(!now_StationName.contains(Cached_StationName))
        {
            tempstring_collector_item<<"车站名"<<Cached_StationName+'W'<<now_StationName+'W';
            tempstring_collector_new[0][1] = "Wrong";
        }
        else
            tempstring_collector_item<<"车站名"<<Cached_StationName+'R'<<now_StationName+'R';
        tempstring_collector_new.push_back(tempstring_collector_item);
        tempstring_collector_item.clear();
        RecordStationName = Cached_StationName;
        tempstring_collector_item<<"应答器"<<RecordBaliseID<<BaliseID;
        tempstring_collector_new.push_back(tempstring_collector_item);
        return tempstring_collector_new;
    };

    if (DesignData::stationVec.empty())//如果没有导入相应的列控数据，那么就退出
        return {};
    auto indexes = Analyze_BaliseMessage::GetPackagePosition("ETCS-72", BPK);
    if(indexes.isEmpty())//如果本包没有E72包，那么就比较之前预告的应答器报文是否与本次车站名相符合
    {
        if(RecordBaliseID.isEmpty())
            return {};
        QString areaID = RecordBaliseID.mid(0,8);
        QString areaID_now = BaliseID.mid(0,8);
        if(areaID_now!=areaID)
        {
            auto result = compare_station_name(Cached_StationName,BaliseID,RecordBaliseID,RecordStationName);
            Cached_StationName.clear();
            RecordBaliseID.clear();
            return result;
        }
        else
            return {};
    }
    if(indexes.size()!=1)
        return {{"number of package E72 is"+QString::number(indexes.size()),"Wrong"}};
    BalisePackage ETCS_72 = BPK[indexes[0]];//从解析结构中获取ETCS-72包
    QVector<Item> Q_DIR_Vec = Analyze_BaliseMessage::FindItem("Q_DIR", ETCS_72);//从ETCS_72包中找到所有的表示验证方向的项，但在本包中，实际上只存在一个项
    if ((Q_DIR_Vec[1].value == 0 && g_TrainDirection == FRONT) || (Q_DIR_Vec[1].value == 1 && g_TrainDirection == BACK))//ETCS-72包只有一个DIR项，作为整个包的运行验证方向，因此只需确定Q_DIR_Vec[0]
        return {};
    /**********************执行到这里，说明符合本函数的执行条件，下面开始比对逻辑********************************/
    QStack<char> temp_stack;
    for (auto it = ETCS_72.rbegin(); it != ETCS_72.rend(); it++)//后向遍历ETCS_72包的内容，将所有描述字符入栈
    {
        if (it->name == "X_TEXT(L_TEXT)")//如果本项描述的是文本字节值，那么字符入栈
        {
            temp_stack.push(static_cast<char>(it->value));
        }
        else//如果已经将所有文本字节值提取出来了
            break;
    }

    QByteArray encodedString;//应答器报文的字符编码为GBK18030字符集，这里需要转换为UNicode编码
    QString Station_Side;
    char first_char = temp_stack.pop();


    if (first_char == '*')//表示文本信息时车站名
    {
        while (!temp_stack.empty())//为需要转换的字符数据赋值
        {
            encodedString.push_back(temp_stack.pop());
        }
        QTextCodec *codec = QTextCodec::codecForName("gb18030");
        Cached_StationName = codec->toUnicode(encodedString);//转换为Unicode编码
        RecordStationName = Cached_StationName;
        RecordBaliseID = BaliseID;
    }
    else if (first_char == '#')//表示文本信息是站台侧
    {
        if (temp_stack.size() != 1)
            return {{"number of character after '#' is not 1,real character size:"+QString::number(temp_stack.size()),"Wrong"}};
        else
            if (temp_stack.top() == 'L' || temp_stack.top() == 'l')
                Station_Side = "左";
            else if (temp_stack.top() == 'R' || temp_stack.top() == 'r')
                Station_Side = "右";
            else
                return {{"character after '#' is neither 'L' nor 'R',but'"+QString(temp_stack.top())+"' instead.","Wrong"}};
    }
    else
    {
        return {{"first character is neither '*' nor '#',but'"+QString(first_char)+"' instead.","Wrong"}};
    }

    if (first_char == '*')//车站名比对
    {
        auto result = compare_station_name(Cached_StationName,BaliseID,RecordBaliseID,RecordStationName);
		if (result.size() <= 1)
			return result;
        if(result[0][1]=="Right")
        {
            Cached_StationName.clear();
            RecordBaliseID.clear();
        }
        return result[0][1]=="Wrong"?QVector<QVector<QString>>{}:result;
    }
    else//站台侧比对
    {
        QVector<QVector<QString>> tempstring_collector_new;
        QVector<QString> tempstring_collector_item;
        tempstring_collector_new.push_back(tempstring_collector_item);
        tempstring_collector_new[0].resize(2);
        tempstring_collector_new[0][0] = BaliseID;
        tempstring_collector_new[0][1] = "Right";

        QString now_StationName = GetStationNameOfBliaseGroup(BaliseID);
        tempstring_collector_item<<""<<"来自应答器的信息"<<"来自数据表的信息";
        tempstring_collector_new<<tempstring_collector_item;
        tempstring_collector_item.clear();
        if(now_StationName.contains(RecordStationName))
            tempstring_collector_item<<"车站名"<<RecordStationName+'R'<<now_StationName+'R';
		else
		{
			tempstring_collector_item << "车站名" << RecordStationName + 'W' << now_StationName + 'W';
			tempstring_collector_new[0][1] = "Wrong";
		}
        tempstring_collector_new<<tempstring_collector_item;
        tempstring_collector_item.clear();
        //获取应答器组名称
        QString SideRecordOfTable;
        BalisePosition BL;
        if (g_LineDirection == UP)
            BL = DesignData::baliseLocationUpMap[BaliseID];
        else if (g_LineDirection == DOWN)
            BL = DesignData::baliseLocationDownMap[BaliseID];
        QString current_balise_name = BL.baliseName.split("-")[0];// ReadFileThread::strFilter(BL.baliseName, QString("-"), None, true);
        for(auto& line:DesignData::stationSideVec)
        {
            if(line.stationName.contains(now_StationName) && line.baliseName == current_balise_name)
            {
                SideRecordOfTable = line.stationSide;
                break;
            }
        }
        if(Station_Side == SideRecordOfTable)
            tempstring_collector_item<<"站台侧"<<Station_Side+'R'<<SideRecordOfTable+'R';
        else
        {
            tempstring_collector_item<<"站台侧"<<Station_Side+'W'<<SideRecordOfTable+'W';
            tempstring_collector_new[0][1] = "Wrong";
        }
        tempstring_collector_new<<tempstring_collector_item;
        return tempstring_collector_new;
    }
}
