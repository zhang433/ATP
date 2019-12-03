#include "rules_zhantai.h"

Rules_ZHANTAI::Rules_ZHANTAI()
{

}

bool Rules_ZHANTAI::convert(QList<QPair<FileInfo, ExcelFile> > &excelFiles, QList<QPair<QString, Sheet> > &All_ConvertSheet)
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
        return true;
    };
    rule[2] = [](QPair<FileInfo, ExcelFile>& sheetfile)->bool
    {
        return true;
    };
    rule[3] = [](QPair<FileInfo, ExcelFile>& sheetfile)->bool
    {
        return true;
    };
    rule[4] = [](QPair<FileInfo, ExcelFile>& sheetfile)->bool
    {
        //检查
        for (auto& sheet : sheetfile.second)
        {
            for (auto& items : sheet.second)
            {
                if(items[4]!="左" && items[4]!="右")
                {
                    w->printerr("站台侧信息无效", RuleCommon::get_rowContent(items),"站台侧信息只能为左或者右");
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
        if (iter->first.fileType == FileType::ZHANTAI)
        {
            files.push_back(qMove(*iter));
            iter = excelFiles.erase(iter);
        }
		else
			iter++;
    }
    if(files.isEmpty())
        w->printinfo("未检测到符合命名规范的站台侧表");
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
        for(auto& sheet: file.second)
        {
            for(auto& line: sheet.second)
            {
                StationSide stationSide;
                stationSide.ID = line[0];
                stationSide.stationName = line[1];
                stationSide.trackName = line[2];
                stationSide.baliseName = line[3];
                stationSide.stationSide = line[4];
                DesignData::stationSideVec.append(stationSide);
            }
        }
    }
    return true;
}
