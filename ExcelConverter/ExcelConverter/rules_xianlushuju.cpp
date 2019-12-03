#include "rules_xianlushuju.h"
#include <QSet>

Rules_XIANLUSHUJU::Rules_XIANLUSHUJU()
{

}

bool Rules_XIANLUSHUJU::convert(QList<QPair<FileInfo, ExcelFile> > &excelFiles, QList<QPair<QString, Sheet> > &All_ConvertSheet)
{
    std::function<bool(QPair<FileInfo, ExcelFile>&)> rule[11];
    rule[0] = [](QPair<FileInfo, ExcelFile>& sheetfile)->bool
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
    rule[1] = [](QPair<FileInfo, ExcelFile>& sheetfile)->bool
    {
        return true;
    };
    rule[2] = [](QPair<FileInfo, ExcelFile>& sheetfile)->bool
    {
        return true;
    };
    rule[3] = [](QPair<FileInfo, ExcelFile>& sheetfile)->bool
    {
        //检查
        for (auto& sheet : sheetfile.second)
        {
            for (auto& items : sheet.second)
            {
                if(!RuleCommon::check_Km(items[3]))
                {
                    w->printerr("信号点里程值无效", RuleCommon::get_rowContent(items));
                    return false;
                }
            }
        }
        return true;
    };
    rule[4] = [](QPair<FileInfo, ExcelFile>& sheetfile)->bool
    {
        static const QSet<QString> table = {"进站信号机","没有信号机","通过信号机","出站信号机","出站口"};
        //检查
        for (auto& sheet : sheetfile.second)
        {
            for (auto& items : sheet.second)
            {
                if(table.find(items[4])==table.end())
                {
                    w->printerr("信号点类型无效", RuleCommon::get_rowContent(items),"只能为字段之一：进站信号机,没有信号机,通过信号机,出站信号机,出站口");
                    return false;
                }
            }
        }
        return true;
    };
    rule[5] = [](QPair<FileInfo, ExcelFile>& sheetfile)->bool
    {
        static const QSet<QString> table = {"机械绝缘节","电气绝缘节"};
        //检查
        for (auto& sheet : sheetfile.second)
        {
            for (auto& items : sheet.second)
            {
                if(table.find(items[5])==table.end())
                {
                    w->printerr("绝缘节类型无效", RuleCommon::get_rowContent(items),"只能为字段之一：机械绝缘节,电气绝缘节");
                    return false;
                }
            }
        }
        return true;
    };
    rule[6] = [](QPair<FileInfo, ExcelFile>& sheetfile)->bool
    {
        return true;
    };
    rule[7] = [](QPair<FileInfo, ExcelFile>& sheetfile)->bool
    {
        return true;
    };
    rule[8] = [](QPair<FileInfo, ExcelFile>& sheetfile)->bool
    {
        //检查
        for (auto& sheet : sheetfile.second)
        {
            for (auto& items : sheet.second)
            {
                if(!RuleCommon::check_Km(items[8]))
                {
                    w->printerr("长度值无效", RuleCommon::get_rowContent(items));
                    return false;
                }
            }
        }
        return true;
    };
    rule[9] = [](QPair<FileInfo, ExcelFile>& sheetfile)->bool
    {
        return true;
    };
    rule[10] = [](QPair<FileInfo, ExcelFile>& sheetfile)->bool
    {
        return true;
    };
    QList<QPair<FileInfo, ExcelFile>> files;
    auto iter = excelFiles.begin();
    //筛选文件
    while (iter != excelFiles.end())
    {
        if (iter->first.fileType == FileType::XIANLUSHUJU)
        {
            files.push_back(qMove(*iter));
            iter = excelFiles.erase(iter);
        }
		else
			iter++;
    }
    if(files.isEmpty())
        w->printinfo("未检测到符合命名规范的线路数据表");
    //掐头去尾
    for (auto& file : files)
    {
        w->printinfo("正在检查文件:" + file.first.fileName);
        if (file.second.size() != 4)
        {
            w->printerr("子表个数必须严格为4");
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
        for(auto& sheet: file.second)
        {
            for(auto& line: sheet.second)
            {
                PathWayData pathWayData;
                pathWayData.ID = line[0];
                pathWayData.stationName = line[1];
                pathWayData.signalSpotName = line[2];
                pathWayData.signalSpotKm = line[3];
                pathWayData.signalSpotType = line[4];
                pathWayData.insulationJointType = line[5];
                pathWayData.pathWayName = line[6];
                pathWayData.pathWayFre= line[7];
                pathWayData.pathWayLen = line[8];
                pathWayData.pathWayCof = line[9];
                pathWayData.remark = line[10];
                switch(sheet.first)
                {
                case SheetType::UP_FRONT:
                    DesignData::pathWayDataUpProVec.push_back(pathWayData);
                    break;
                case SheetType::DOWN_FRONT:
                    DesignData::pathWayDataDownProVec.push_back(pathWayData);
                    break;
                case SheetType::UP_BACK:
                    DesignData::pathWayDataUpBackVec.push_back(pathWayData);
                    break;
                case SheetType::DOWN_BACK:
                    DesignData::pathWayDataDownBackVec.push_back(pathWayData);
                    break;
                default:
                    w->printerr(file.first.fileName+"子表名称不符合规范(须包含上下行及正反向信息)");
                    return false;
                }
            }
        }
    }
    return true;
}
