#ifndef RULES_SUDU_H
#define RULES_SUDU_H
#include "rulecommon.h"

class Rules_SUDU
{
public:
    Rules_SUDU();
    static bool convert(QList<QPair<FileInfo, ExcelFile>> &excelFiles, QList<QPair<QString, Sheet>>& All_ConvertSheet);
};

#endif // RULES_SUDU_H
