#include "rules_sudu.h"

Rules_SUDU::Rules_SUDU()
{

}

bool Rules_SUDU::convert(QList<QPair<FileInfo, ExcelFile> > &excelFiles, QList<QPair<QString, Sheet> > &All_ConvertSheet)
{
    std::function<bool(QPair<FileInfo, ExcelFile>&)> rule[5];
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
        //检查
        for (auto& sheet : sheetfile.second)
        {
            for (auto& items : sheet.second)
            {
                if(!RuleCommon::check_Km(items[1]))
                {
                    w->printerr("速度值无效", RuleCommon::get_rowContent(items));
                    return false;
                }
            }
        }
        return true;
    };
    rule[2] = [](QPair<FileInfo, ExcelFile>& sheetfile)->bool
    {
        //检查
        for (auto& sheet : sheetfile.second)
        {
            for (auto& items : sheet.second)
            {
                if(!RuleCommon::check_Km(items[2]))
                {
                    w->printerr("长度无效", RuleCommon::get_rowContent(items));
                    return false;
                }
            }
        }
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
                    w->printerr("终点里程值无效", RuleCommon::get_rowContent(items));
                    return false;
                }
            }
        }
        return true;
    };
    rule[4] = [](QPair<FileInfo, ExcelFile>& sheetfile)->bool
    {
        //检查
        for (auto& sheet : sheetfile.second)
        {
            for (auto& items : sheet.second)
            {
                if(!items[4].isEmpty() && !RuleCommon::check_Km(items[4]))
                {
                    w->printerr("备注无效", RuleCommon::get_rowContent(items),"备注只能为公里标或者为空");
                    return false;
                }
            }
        }
        return true;
    };
    QList<QPair<FileInfo, ExcelFile>> files;
    auto iter = excelFiles.begin();
    //筛选文件
    while (iter != excelFiles.end())
    {
        if (iter->first.fileType == FileType::SUDU)
        {
            files.push_back(qMove(*iter));
            iter = excelFiles.erase(iter);
        }
		else
			iter++;
    }
    if(files.isEmpty())
        w->printinfo("未检测到符合命名规范的线路速度表");
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
                PathWaySpeed pathWaySpeed;
                pathWaySpeed.ID = line[0];
                pathWaySpeed.speed = line[1];
                pathWaySpeed.len = line[2];
                pathWaySpeed.endKm = line[3];
                pathWaySpeed.remark = line[4];
                switch(sheet.first)
                {
                case SheetType::DOWN_FRONT:
                    DesignData::pathWaySpeedDownVec.push_back(pathWaySpeed);
                    break;
                case SheetType::DOWN_BACK:
                    DesignData::pathWaySpeedDownBackVec.push_back(pathWaySpeed);
                    break;
                case SheetType::UP_FRONT:
                    DesignData::pathWaySpeedUpVec.push_back(pathWaySpeed);
                    break;
                case SheetType::UP_BACK:
                    DesignData::pathWaySpeedUpBackVec.push_back(pathWaySpeed);
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
