#ifndef RULES_XIANLUSHUJU_H
#define RULES_XIANLUSHUJU_H
#include "rulecommon.h"

class Rules_XIANLUSHUJU
{
public:
    Rules_XIANLUSHUJU();
    static bool convert(QList<QPair<FileInfo, ExcelFile>> &excelFiles, QList<QPair<QString, Sheet>>& All_ConvertSheet);
};

#endif // RULES_XIANLUSHUJU_H
