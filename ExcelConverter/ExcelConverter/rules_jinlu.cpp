#include "rules_jinlu.h"
#include <QSet>

Rules_JINLU::Rules_JINLU()
{

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
QString strFilter(QString input, QString filter, FilterType filtertype, bool flag)
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


bool Rules_JINLU::convert(QList<QPair<FileInfo, ExcelFile> > &excelFiles, QList<QPair<QString, Sheet> > &All_ConvertSheet)
{
    std::function<bool(QPair<FileInfo, ExcelFile>&)> rule[14];
    rule[0] = [](QPair<FileInfo, ExcelFile>& sheetfile)->bool//序号
    {
        //检查
        for (auto& sheet : sheetfile.second)
        {
            for (auto& items : sheet.second)
            {
                if(!RuleCommon::check_Order(items[0]))
                {
                    w->printerr("序号无法有效转换", RuleCommon::get_rowContent(items));
                    return false;
                }
            }
        }
        return true;
    };
	rule[1] = [](QPair<FileInfo, ExcelFile>& sheetfile)->bool//应答器
	{
        //检查
        for (auto& sheet : sheetfile.second)
        {
            for (auto& items : sheet.second)
            {
                if (items[1]!="-" && !RuleCommon::check_balishID(items[1]))
                {
                    w->printerr("应答器编号无效", RuleCommon::get_rowContent(items));
                    return false;
                }
            }
        }
		return true;
	};
    rule[2] = [](QPair<FileInfo, ExcelFile>& sheetfile)->bool//进路编号
    {
        //检查
        for (auto& sheet : sheetfile.second)
        {
            for (auto& items : sheet.second)
            {
                if(!RuleCommon::check_Km(items[2]))
                {
                    w->printerr("进路编号无效", RuleCommon::get_rowContent(items));
                    return false;
                }
            }
        }
        return true;
    };
    rule[3] = [](QPair<FileInfo, ExcelFile>& sheetfile)->bool//进路
    {
        return true;
    };
    rule[4] = [](QPair<FileInfo, ExcelFile>& sheetfile)->bool//进路类型
    {
        return true;
    };
    rule[5] = [](QPair<FileInfo, ExcelFile>& sheetfile)->bool//名称
    {
        return true;
    };
    rule[6] = [](QPair<FileInfo, ExcelFile>& sheetfile)->bool//显示
    {
		return true;
    };
    rule[7] = [](QPair<FileInfo, ExcelFile>& sheetfile)->bool//终端信号机名称
    {
        return true;
    };
    rule[8] = [](QPair<FileInfo, ExcelFile>& sheetfile)->bool//经过应答器
    {
        //检查
        for (auto& sheet : sheetfile.second)
        {
            for (auto& items : sheet.second)
            {
                if (items[8] == "-")
                    continue;
                auto strs = items[8].split(",");
                if (strs.size() != 3 && strs.size() != 2 && strs.size()!=1)
                {
                    w->printerr("经过应答器无效", RuleCommon::get_rowContent(items));
                    return false;
                }
                for (auto& str : strs)
                {
                    auto nums = str.split("/");
                    if (nums.size() != 2)
                    {
                        w->printerr("经过应答器无效", RuleCommon::get_rowContent(items));
                        return false;
                    }
                    if (nums[0].size() != 3)
                    {
                        w->printerr("经过应答器无效", RuleCommon::get_rowContent(items));
                        return false;
                    }
                    bool ok = false;
                    nums[0].toUInt(&ok);
                    if(!ok)
                    {
                        w->printerr("经过应答器无效", RuleCommon::get_rowContent(items));
                        return false;
                    }
                    nums[1].toUInt(&ok);
                    if (!ok)
                    {
                        w->printerr("经过应答器无效", RuleCommon::get_rowContent(items));
                        return false;
                    }
                }
            }
        }
        return true;
    };
    rule[9] = [](QPair<FileInfo, ExcelFile>& sheetfile)->bool//道岔
    {
        return true;
    };
    rule[10] = [](QPair<FileInfo, ExcelFile>& sheetfile)->bool//线路速度
    {
        //检查
        for (auto& sheet : sheetfile.second)
        {
            for (auto& items : sheet.second)
            {
                auto strs = items[10].split(",");
                for (auto str : strs)
                {
                    auto nums = str.split("/");
                    if (nums.size() != 2)
                    {
                        w->printerr("线路速度无效", RuleCommon::get_rowContent(items));
                        return false;
                    }
                    bool ok = false;
                    nums[0].toUInt(&ok);
                    if (!ok)
                    {
                        w->printerr("线路速度无效", RuleCommon::get_rowContent(items));
                        return false;
                    }
                    nums[1].toUInt(&ok);
                    if (!ok)
                    {
                        w->printerr("线路速度无效", RuleCommon::get_rowContent(items));
                        return false;
                    }
                }
            }
        }
        return true;
    };
    rule[11] = [](QPair<FileInfo, ExcelFile>& sheetfile)->bool//轨道区段
    {
        //检查
        for (auto& sheet : sheetfile.second)
        {
            for (auto& items : sheet.second)
            {
                auto strs = items[11].split(",");
                for (auto str : strs)
                {
                    auto nums = str.split("\\");
                    if (nums.size() != 4)
                    {
                        w->printerr("轨道区段信息无效", RuleCommon::get_rowContent(items));
                        return false;
                    }
                    bool ok = false;
                    nums[0].toUInt(&ok);
                    if (!ok)
                    {
                        w->printerr("轨道区段信息无效", RuleCommon::get_rowContent(items));
                        return false;
                    }
                    static const QSet<QString> table = { "进站信号机","没有信号机","通过信号机","出站信号机","出站口" };
                    if (table.find(nums[2]) == table.end())
                    {
                        w->printerr("轨道区段信息无效", RuleCommon::get_rowContent(items),"信号机类型只能为字段之一：进站信号机,没有信号机,通过信号机,出战信号机,出站口");
                        return false;
                    }
                }
            }
        }
        return true;
    };
    rule[12] = [](QPair<FileInfo, ExcelFile>& sheetfile)->bool//灾害防护
    {
        return true;
    };
    rule[13] = [](QPair<FileInfo, ExcelFile>& sheetfile)->bool//备注
    {
        return true;
    };
    QList<QPair<FileInfo, ExcelFile>> files;
    auto iter = excelFiles.begin();
    //筛选文件
    while (iter != excelFiles.end())
    {
        if (iter->first.fileType == FileType::JINLU)
        {
            files.push_back(qMove(*iter));
            iter = excelFiles.erase(iter);
        }
		else
			iter++;
    }
    if(files.isEmpty())
        w->printinfo("未检测到符合命名规范的进路表");
    //掐头去尾
    for (auto& file : files)
    {
        w->printinfo("正在检查文件:" + file.first.fileName);
        if (file.second.size() != 1)
        {
            w->printerr("子表个数必须严格为1");
            return false;
        }
        if (!RuleCommon::check_column(file, sizeof(rule) / sizeof(rule[0])))
        {
            w->printerr("请检查每一张子表的列数是否严格为"+QString::number(sizeof(rule) / sizeof(rule[0]))+"（包括隐藏列）");
            return false;
        }
        for (unsigned int i = 0; i < sizeof(rule) / sizeof(rule[0]); ++i)
        {
            if (!(rule[i])(file))
                return false;
        }
    }
    for(auto& file : files)
    {
        AccessRode_Sheet temp_accessRode;
        temp_accessRode.sheet_name = file.first.fileName;
        //遍历每行，填充结构体信息并放入Vector
        QString hold_str_balishID = QString("-");
        QString hold_str_balish = QString("/,");
        QString hold_str_speed = QString("/,");
        QString hold_str_pathway = QString("\\-,");
        for(auto& sheet: file.second)
        {
            for(auto& line: sheet.second)
            {
                AccessRode accseeRode;
                accseeRode.ID = line[0];
                accseeRode.baliseID = strFilter(line[1], hold_str_balishID, FilterType::Num, false);
                if (accseeRode.baliseID.size() != 12 && accseeRode.baliseID.size() != 14)//应答器编号为组内编号时应该是严格的14个字符，为组编号时应该是严格的12字符
                    accseeRode.baliseID.clear();
                accseeRode.routeID = line[2];
                accseeRode.route = line[3];
                accseeRode.routeType = line[4];
                accseeRode.annBeginName = line[5];
                accseeRode.annBeginCodeOrderMax = line[6];
                accseeRode.annunciatorEndName = line[7];

                //考虑输入逗号时的中英文混输入情况
                accseeRode.balise = line[8];
                accseeRode.balise.replace("，", ",");
                accseeRode.balise = strFilter(accseeRode.balise, hold_str_balish, FilterType::Num, false);

                accseeRode.turnout = line[9];

                //考虑输入逗号时的中英文混输入情况
                accseeRode.speed = line[10];
                accseeRode.speed.replace("，", ",");
                accseeRode.speed = strFilter(accseeRode.speed, hold_str_speed, FilterType::Num, false);

                //考虑输入逗号时的中英文混输入情况
                accseeRode.pathWay = line[11];
                accseeRode.pathWay.replace("，", ",");
                accseeRode.pathWay = strFilter(accseeRode.pathWay, hold_str_pathway, (FilterType)(FilterType::Num | FilterType::Chinese | FilterType::LowerLetter | FilterType::UpperLetter), false);

                accseeRode.disasterPreLen = line[12];
                accseeRode.remark = line[13];
                temp_accessRode.sheet.push_back(accseeRode);

            }
            //进行进路标的出站信息补全
            for (unsigned int i = 0; i < temp_accessRode.sheet.size(); i++)
            {
                if (temp_accessRode.sheet[i].balise.size() < 3 && temp_accessRode.sheet[i].baliseID.size())//当该表格的内容的字符数小于3的时候，就认为是空表，因为该表内容可能为"-"或者没有数据，或者其他形式
                {
                    QStringList list_str = temp_accessRode.sheet[i].baliseID.split("-");//获取其应答器组编号
                    for (unsigned int j = 0; j < temp_accessRode.sheet.size(); j++)
                    {
                        //两个判定条件：1，本条的应答器组编号和遍历找到的目标条的链接应答器组编号相同。2，本条的终端信号机的名称与目标条的始端信号机的名称相符
                        if (list_str[3] == temp_accessRode.sheet[j].balise.split("/")[0] && temp_accessRode.sheet[i].annunciatorEndName == temp_accessRode.sheet[j].annBeginName && temp_accessRode.sheet[i].baliseID.mid(0,8)==temp_accessRode.sheet[j].baliseID.mid(0,8))
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
                DesignData::accessRodeMap.insert(temp_accessRode.sheet[i].baliseID.mid(0,12), am);
            }
        }
    }
    return true;
}
