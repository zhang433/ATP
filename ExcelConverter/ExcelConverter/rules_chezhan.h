#ifndef RULES_CHEZHAN_H
#define RULES_CHEZHAN_H
#include "rules_yingdaqiweizhi.h"

class Rules_CHEZHAN
{
public:
    Rules_CHEZHAN();
    static bool convert(QList<QPair<FileInfo, ExcelFile>> &excelFiles, QList<QPair<QString, Sheet>>& All_ConvertSheet);
};

#endif // RULES_CHEZHAN_H
