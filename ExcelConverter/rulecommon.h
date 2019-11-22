#pragma once
#include "mainwindow.h"
#include <QString>
#include <functional>


class RuleCommon
{
public:
	RuleCommon();
	static bool check_column(QPair<FileInfo, ExcelFile>& sheetfile, int column_num);
	static QString get_rowContent(SheetLine& items);
	static bool check_balishID(const QString& balishID);
	static bool check_Km(const QString& balishID);
    static bool check_Order(const QString& order);
	~RuleCommon();
private:
};

