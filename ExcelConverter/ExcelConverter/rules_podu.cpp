#include "rules_podu.h"

Rules_PODU::Rules_PODU()
{

}

bool Rules_PODU::convert(QList<QPair<FileInfo, ExcelFile> > &excelFiles, QList<QPair<QString, Sheet> > &All_ConvertSheet)
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
        bool ok = false;
        for (auto& sheet : sheetfile.second)
        {
            for (auto& items : sheet.second)
            {
                items[1].toDouble(&ok);
                if(!ok)
                {
                    w->printerr("坡度值无法有效转换", RuleCommon::get_rowContent(items));
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
                if(!RuleCommon::check_Order(items[2]))
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
                    w->printerr("终点里程无效", RuleCommon::get_rowContent(items));
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
                if(!items[4].isEmpty() && !RuleCommon::check_Order(items[4]))
                {
                    w->printerr("备注无效", RuleCommon::get_rowContent(items), "备注只能为公里标或者为空");
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
        if (iter->first.fileType == FileType::PODU)
        {
            files.push_back(qMove(*iter));
            iter = excelFiles.erase(iter);
        }
		else
			iter++;
    }
    if(files.isEmpty())
        w->printinfo("未检测到符合命名规范的坡度信息表");
    //掐头去尾
    for (auto& file : files)
    {
        w->printinfo("正在检查文件:" + file.first.fileName);
        if (file.second.size() != 2)
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
                return false;
        }
    }

    for(auto& file : files)
    {
        for(auto& sheet: file.second)
        {
            for(auto& line: sheet.second)
            {
                Grade grade;
                grade.ID = line[0];
                grade.grade = line[1];
                grade.len = line[2];
                grade.endKm = line[3];
                grade.remark = line[4];
                switch(sheet.first)
                {
                case SheetType::DOWN:
                    DesignData::gradeDownProVec.push_back(grade);
                    break;
                case SheetType::UP:
                    DesignData::gradeUpProVec.push_back(grade);
                    break;
                default:
                    w->printerr(file.first.fileName+"子表名称不符合规范(须包含上行或下行字样，且不能出现正反向)");
                    return false;
                }
            }
        }

    }
    return true;
}
