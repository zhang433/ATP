﻿#include "rules_guofenxiang.h"

Rules_GUOFENXIANG::Rules_GUOFENXIANG()
{

}

bool Rules_GUOFENXIANG::convert(QList<QPair<FileInfo, ExcelFile> > &excelFiles, QList<QPair<QString, Sheet> > &All_ConvertSheet)
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
                    w->printerr("起点里程无效", RuleCommon::get_rowContent(items));
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
                    w->printerr("终点里程无效", RuleCommon::get_rowContent(items));
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
                    w->printerr("长度无效", RuleCommon::get_rowContent(items));
                    return false;
                }
            }
        }
        return true;
    };
    rule[4] = [](QPair<FileInfo, ExcelFile>& sheetfile)->bool
    {
        return true;
    };
    QList<QPair<FileInfo, ExcelFile>> files;
    auto iter = excelFiles.begin();
    //筛选文件
    while (iter != excelFiles.end())
    {
        if (iter->first.fileType == FileType::FENXIANG)
        {
            files.push_back(qMove(*iter));
            iter = excelFiles.erase(iter);
        }
		else
			iter++;
    }
    if(files.isEmpty())
        w->printinfo("未检测到符合命名规范的过分相数据表");
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
                NeutralSection neutralSection;
                neutralSection.ID = line[0];
                neutralSection.beganKm = line[1];
                neutralSection.endKm = line[2];
                neutralSection.len = line[3];
                neutralSection.remark = line[4];
                if(sheet.first == SheetType::DOWN)
                {
                    DesignData::neutralSectionDownVec.push_back(neutralSection);
                }
                else if(sheet.first == SheetType::UP)
                {
                    DesignData::neutralSectionUpVec.push_back(neutralSection);
                }
                else
                {
                    w->printerr(file.first.fileName+"子表名称不符合规范(须包含上行、下行字样)");
                    return false;
                }
            }
        }
    }
    return true;
}
