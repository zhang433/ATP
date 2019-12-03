#ifndef RULES_YINGDAQIWEIZHI_H
#define RULES_YINGDAQIWEIZHI_H
#include <QString>
#include <QVector>
#include <QPair>
#include "rulecommon.h"

class Rules_YINGDAQIWEIZHI
{
public:
    Rules_YINGDAQIWEIZHI();
    static bool convert(QList<QPair<FileInfo, ExcelFile>> &excelFiles, QList<QPair<QString, Sheet>>& All_ConvertSheet);
};

#endif // RULES_YINGDAQIWEIZHI_H
