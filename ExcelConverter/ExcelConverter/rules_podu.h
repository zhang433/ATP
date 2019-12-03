#ifndef RULES_PODU_H
#define RULES_PODU_H
#include "rulecommon.h"

class Rules_PODU
{
public:
    Rules_PODU();
    static bool convert(QList<QPair<FileInfo, ExcelFile>> &excelFiles, QList<QPair<QString, Sheet>>& All_ConvertSheet);
};

#endif // RULES_PODU_H
