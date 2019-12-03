#ifndef RULES_JINLU_H
#define RULES_JINLU_H
#include "rulecommon.h"

enum FilterType {
    None = 0x00,
    Num = 0x01,
    LowerLetter = 0x02,
    UpperLetter = 0x04,
    Chinese = 0x80
};

class Rules_JINLU
{
public:
    Rules_JINLU();
    static bool convert(QList<QPair<FileInfo, ExcelFile>> &excelFiles, QList<QPair<QString, Sheet>>& All_ConvertSheet);
};

#endif // RULES_JINLU_H
