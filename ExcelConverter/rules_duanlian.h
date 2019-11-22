#ifndef RULES_DUANLIAN_H
#define RULES_DUANLIAN_H
#include "rulecommon.h"

class Rules_DUANLIAN
{
public:
    Rules_DUANLIAN();
    static bool convert(QList<QPair<FileInfo, ExcelFile>> &excelFiles, QList<QPair<QString, Sheet>>& All_ConvertSheet);
};

#endif // RULES_DUANLIAN_H
