#include "rules_chezhan.h"

Rules_CHEZHAN::Rules_CHEZHAN()
{

}

bool Rules_CHEZHAN::convert(QList<QPair<FileInfo, ExcelFile> > &excelFiles, QList<QPair<QString, Sheet> > &All_ConvertSheet)
{
    std::function<bool(QPair<FileInfo, ExcelFile>&)> rule[5];
    rule[0] = [](QPair<FileInfo, ExcelFile>& sheetfile)->bool
    {
        //检查
        for (auto& sheet : sheetfile.second)
        {
            for (auto& items : sheet)
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
        //检查
        for (auto& sheet : sheetfile.second)
        {
            for (auto& items : sheet)
            {
                if(items[2].size()!=3 || !RuleCommon::check_Order(items[2]))
                {
                    w->printerr("大区编号无效", RuleCommon::get_rowContent(items), "示例：075");
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
            for (auto& items : sheet)
            {
                if(items[3].size()!=1 || !RuleCommon::check_Order(items[3]))
                {
                    w->printerr("小区编号无效", RuleCommon::get_rowContent(items), "示例：5");
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
            for (auto& items : sheet)
            {
                if(items[4].size()!=2 || !RuleCommon::check_Order(items[4]))
                {
                    w->printerr("车站编号无效", RuleCommon::get_rowContent(items), "示例：11");
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
		if (iter->first.fileType == CHEZHAN)
		{
			files.push_back(qMove(*iter));
			iter = excelFiles.erase(iter);
		}
		else
			iter++;
    }
    if(files.isEmpty())
        w->printinfo("未检测到符合命名规范的车站信息表");
    //掐头去尾
    for (auto& file : files)
    {
        w->printinfo("正在检查文件:" + file.first.fileName);
        if (file.second.size() != 1)
        {
            w->printerr("子表个数必须严格为2");
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
                return {};
        }
    }
    Sheet sheet;
    for (auto& file : files)
    {
        for (auto& line: file.second)
        {
            sheet.append(line);
        }
    }
    All_ConvertSheet.append({ "车站信息汇总",sheet});
    return true;
}
