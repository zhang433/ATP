#ifndef RULES_JINLU_H
#define RULES_JINLU_H
#include "rulecommon.h"

class Rules_JINLU
{
public:
    Rules_JINLU();
    static bool convert(QList<QPair<FileInfo, ExcelFile>> &excelFiles, QList<QPair<QString, Sheet>>& All_ConvertSheet);
};

#endif // RULES_JINLU_H
