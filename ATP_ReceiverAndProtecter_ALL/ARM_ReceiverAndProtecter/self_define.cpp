#include "self_define.h"
#include "qdebug.h"

QMutex Print_MTX, ReportFile_MTX;
QString g_fileRootPath;
HistoryFile DMSCompareResultFile, DMSReportFile;
