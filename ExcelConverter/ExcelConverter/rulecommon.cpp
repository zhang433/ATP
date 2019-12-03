#include "rulecommon.h"
#include <QList>


RuleCommon::RuleCommon()
{
}

bool RuleCommon::check_column(QPair<FileInfo, ExcelFile>& sheetfile, int column_num)
{
	//掐头去尾
	for (auto& sheet : sheetfile.second)//对于每一张子表
	{
		//掐头
		{
            auto iter = sheet.second.begin();
			bool ok = false;
            while (iter != sheet.second.end())//针对每一行操作
			{
				if (iter->isEmpty())
					break;
				iter->begin()->toUInt(&ok);
				if (ok)
					break;
                iter = sheet.second.erase(iter);
			}
		}
		//去尾
		{
            auto iter = sheet.second.rbegin();
			bool ok = false;
            while (iter != sheet.second.rend())//针对每一行操作
			{
				if (iter->isEmpty())
					break;
				iter->begin()->toUInt(&ok);
				if (ok)
					break;
				iter++;
                sheet.second.pop_back();
			}
		}
	}
	//检查列数
	for (auto& sheet : sheetfile.second)
	{
        for (auto& items : sheet.second)
		{
			if (items.size() != column_num)
			{
				return false;
			}
		}
	}
	return true;
}

bool RuleCommon::check_balishID(const QString& balishID)
{
	if (balishID.size() != 12 && balishID.size() != 14)
		return false;
	auto nums = balishID.split('-');
	if (balishID.size() == 12 && nums.size() != 4)
		return false;
	else if (balishID.size() == 14 && nums.size() != 5)
		return false;
	static const int sizes[5] = { 3,1,2,3,1 };
	for (int i = 0; i < nums.size(); ++i)
	{
		if (nums[i].size() != sizes[i])
			return false;
	}
	bool ok = false;
	for (auto& str : nums)
	{
		str.toUInt(&ok);
		if (!ok)
			return false;
	}
	return true;
}

QString RuleCommon::get_rowContent(SheetLine& items)
{
	QString str;
	for (auto& s : items)
	{
		str.append(s + "|");
	}
	return str;
}

bool RuleCommon::check_Km(const QString& KM)
{
	bool ok = false;
	KM.toUInt(&ok);
    if(ok)
        return true;
    else
    {
        auto split_strs = KM.split('+');
        if(split_strs.size()!=2)
            return false;
        if(split_strs[0].size()==0)
            return false;
        if(split_strs[0][0]!='K')
            return false;
        split_strs[0].remove(0,1);
        split_strs[0].toUInt(&ok);
        if(!ok)
            return false;
        split_strs[1].toUInt(&ok);
        if(!ok)
            return false;
        return true;
    }
}

bool RuleCommon::check_Order(const QString &order)
{
    bool ok = false;
    order.toUInt(&ok);
    if(!ok)
        return false;
    return true;
}

RuleCommon::~RuleCommon()
{
}
