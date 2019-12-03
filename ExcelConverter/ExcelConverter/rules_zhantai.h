#ifndef RULES_ZHANTAI_H
#define RULES_ZHANTAI_H
#include "rulecommon.h"

class Rules_ZHANTAI
{
public:
    Rules_ZHANTAI();
    static bool convert(QList<QPair<FileInfo, ExcelFile>> &excelFiles, QList<QPair<QString, Sheet>>& All_ConvertSheet);
};

#endif // RULES_ZHANTAI_H
