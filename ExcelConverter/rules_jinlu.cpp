#include "rules_jinlu.h"
#include <QSet>

Rules_JINLU::Rules_JINLU()
{

}

bool Rules_JINLU::convert(QList<QPair<FileInfo, ExcelFile> > &excelFiles, QList<QPair<QString, Sheet> > &All_ConvertSheet)
{
    std::function<bool(QPair<FileInfo, ExcelFile>&)> rule[14];
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
        //检查
        for (auto& sheet : sheetfile.second)
        {
            for (auto& items : sheet)
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
            for (auto& items : sheet)
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
            for (auto& items : sheet)
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
            for (auto& items : sheet)
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
            for (auto& items : sheet)
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
        if (iter->first.fileType == JINLU)
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
    All_ConvertSheet.append({ "进路信息汇总",qMove(sheet) });
    return true;
}
