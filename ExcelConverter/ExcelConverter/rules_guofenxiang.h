#ifndef RULES_GUOFENXIANG_H
#define RULES_GUOFENXIANG_H
#include "rulecommon.h"

class Rules_GUOFENXIANG
{
public:
    Rules_GUOFENXIANG();
    static bool convert(QList<QPair<FileInfo, ExcelFile>> &excelFiles, QList<QPair<QString, Sheet>>& All_ConvertSheet);
};

#endif // RULES_GUOFENXIANG_H
