#include "rules_duanlian.h"

Rules_DUANLIAN::Rules_DUANLIAN()
{

}

bool Rules_DUANLIAN::convert(QList<QPair<FileInfo, ExcelFile> > &excelFiles, QList<QPair<QString, Sheet> > &All_ConvertSheet)
{
    std::function<bool(QPair<FileInfo, ExcelFile>&)> rule[8];
    rule[0] = [](QPair<FileInfo, ExcelFile>& sheetfile)->bool//序号
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
    rule[1] = [](QPair<FileInfo, ExcelFile>& sheetfile)->bool//应答器
    {
        return true;
    };
    rule[2] = [](QPair<FileInfo, ExcelFile>& sheetfile)->bool//进路编号
    {
        //检查
        for (auto& sheet : sheetfile.second)
        {
            for (auto& items : sheet)
            {
                if(items[2]!="下行"&&items[2]!="上行")
                {
                    w->printerr("行别无效", RuleCommon::get_rowContent(items),"只能为字段之一：下行，上行");
                    return false;
                }
            }
        }
        return true;
    };
    rule[3] = [](QPair<FileInfo, ExcelFile>& sheetfile)->bool//进路
    {
        //检查
        for (auto& sheet : sheetfile.second)
        {
            for (auto& items : sheet)
            {
                if(items[3]!="长链"&&items[3]!="短链")
                {
                    w->printerr("断链类型无效", RuleCommon::get_rowContent(items),"只能为字段之一：长链，短链");
                    return false;
                }
            }
        }
        return true;
    };
    rule[4] = [](QPair<FileInfo, ExcelFile>& sheetfile)->bool//进路
    {
        //检查
        for (auto& sheet : sheetfile.second)
        {
            for (auto& items : sheet)
            {
                if(!RuleCommon::check_Km(items[4]))
                {
                    w->printerr("起点里程无效", RuleCommon::get_rowContent(items));
                    return false;
                }
            }
        }
        return true;
    };
    rule[5] = [](QPair<FileInfo, ExcelFile>& sheetfile)->bool//进路类型
    {
        //检查
        for (auto& sheet : sheetfile.second)
        {
            for (auto& items : sheet)
            {
                if(!RuleCommon::check_Km(items[5]))
                {
                    w->printerr("终点里程无效", RuleCommon::get_rowContent(items));
                    return false;
                }
            }
        }
        return true;
    };
    rule[6] = [](QPair<FileInfo, ExcelFile>& sheetfile)->bool//名称
    {
        //检查
        for (auto& sheet : sheetfile.second)
        {
            for (auto& items : sheet)
            {
                if(!items[6].isEmpty()&&!RuleCommon::check_Km(items[6]))
                {
                    w->printerr("长链值无效", RuleCommon::get_rowContent(items));
                    return false;
                }
            }
        }
        return true;
    };
    rule[7] = [](QPair<FileInfo, ExcelFile>& sheetfile)->bool//显示
    {
        //检查
        for (auto& sheet : sheetfile.second)
        {
            for (auto& items : sheet)
            {
                if(!items[7].isEmpty()&&!RuleCommon::check_Km(items[7]))
                {
                    w->printerr("短链值无效", RuleCommon::get_rowContent(items));
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
        if (iter->first.fileType == DUANLIAN)
        {
            files.push_back(qMove(*iter));
            iter = excelFiles.erase(iter);
        }
        else
            iter++;
    }
    if(files.isEmpty())
        w->printinfo("未检测到符合命名规范的断链表");
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
    All_ConvertSheet.append({ "断链信息汇总",qMove(sheet) });
    return true;
}
