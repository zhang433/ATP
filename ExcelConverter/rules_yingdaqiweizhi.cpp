#include "rules_yingdaqiweizhi.h"
#include <functional>
#include <QDebug>
#include <qalgorithms.h>

Rules_YINGDAQIWEIZHI::Rules_YINGDAQIWEIZHI()
{

}

bool Rules_YINGDAQIWEIZHI::convert(QList<QPair<FileInfo, ExcelFile>> &excelFiles, QList<QPair<QString, Sheet>>& All_ConvertSheet)
{
    std::function<bool(QPair<FileInfo, ExcelFile>&)> rule[8];
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
				if (!RuleCommon::check_balishID(items[2]))
				{
					w->printerr("应答器编号无效", RuleCommon::get_rowContent(items), "示例：075-5-14-001-1");
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
				if (!RuleCommon::check_Km(items[3]))
				{
					w->printerr("里程标识不合法，无法转换为数字", RuleCommon::get_rowContent(items));
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
				if (items[4]!="有源"&&items[4]!="无源")
				{
					w->printerr("设备类型不合法", RuleCommon::get_rowContent(items), "只能为有源或者无源");
					return false;
				}
			}
		}
		return true;
	};
    rule[5] = [](QPair<FileInfo, ExcelFile>& sheetfile)->bool
	{
		static QSet<QString> s = { "CZ","DW","FCZ","FDW","FJZ","FQ","JZ","Q","ZJ1","ZJ2" };
		//检查
		for (auto& sheet : sheetfile.second)
		{
			for (auto& items : sheet)
			{
				if (items[4] != "有源"&&items[4] != "无源")
				{
					w->printerr("用途不合法", RuleCommon::get_rowContent(items), "用途为以下字段之一：CZ,DW,FCZ,FDW,FJZ,FQ,JZ,Q,ZJ1,ZJ2");
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
	QList<QPair<FileInfo, ExcelFile>> files;
	auto iter = excelFiles.begin();
	//筛选文件
	while (iter != excelFiles.end())
	{
		if (iter->first.fileType == YINGDAQIWEIZHI)
		{
			files.push_back(qMove(*iter));
			iter = excelFiles.erase(iter);
		}
		else
			iter++;
	}
    if(files.isEmpty())
        w->printinfo("未检测到符合命名规范的应答器位置表");
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
	All_ConvertSheet.append({ "应答器位置信息汇总",qMove(sheet) });
    return true;
}
