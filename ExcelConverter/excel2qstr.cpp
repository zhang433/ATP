#include "excel2qstr.h"
#include <QString>
#include <QDebug>
#include <QtAlgorithms>
const QMap<QString,FileType> Excel2QStr::key_words = {{"进路",JINLU},{"车站",CHEZHAN},{"坡度",PODU},{"分相",FENXIANG},{"线路数据",XIANLUSHUJU},{"速度",SUDU},{"应答器位置",YINGDAQIWEIZHI},{"断链",DUANLIAN},{"站台",ZHANTAI}};

bool operator<(const FileInfo& l,const FileInfo& r)
{
    return l.fileName<r.fileName;
}

Excel2QStr::Excel2QStr(QStringList list,QString& info):
    excel("Excel.Application"),
    info(info)
{
    this->list = list;
    excel.setProperty("Visible", false); //隐藏打开的excel文件界面
    workbooks = excel.querySubObject("WorkBooks");
}

Sheet Excel2QStr::castVariant2ListListVariant(const QVariant &var)
{
    QList<QList<QVariant> > res;
    QVariantList varRows = var.toList();
    if(varRows.isEmpty())
    {
        return {};
    }
    const int rowCount = varRows.size();
    QVariantList rowData;
    for(int i=0;i<rowCount;++i)
    {
        rowData = varRows[i].toList();
        res.push_back(rowData);
    }
    Sheet ret;
    for(int i=0;i<res.size();++i)
    {
        ret.push_back({});
        for(int j=0;j<res[i].size();++j)
        {
            ret.back().push_back(res[i][j].toString());
        }
    }
    return ret;
}

QList<QPair<FileInfo, ExcelFile>> Excel2QStr::getQStrTable()
{
    QList<QPair<FileInfo, ExcelFile>> m;//文件-表格-行-列
    for(auto& str:list)//针对每一个文件
    {
        ExcelFile sheets;
        FileInfo FI;
        FI.fileName = str;
        FI.fileType = NONE;
        for(auto iter = key_words.begin();iter!=key_words.end();++iter)
        {
            if(str.contains(iter.key()))
            {
                FI.fileType = iter.value();
                break;
            }
        }
        int sheetCount = 0;
        QAxObject* workbook = workbooks->querySubObject("Open(QString, QVariant)", str); //打开文件
        {
            QAxObject* activeWorkbook = excel.querySubObject("ActiveWorkBook");//获取活动工作簿
            if (activeWorkbook == nullptr)
            {
                info.append("获取子表个数失败："+str);
                return {};
            }
            QAxObject *worksheets = activeWorkbook->querySubObject("WorkSheets");//获取所有的工作表
            sheetCount = worksheets->property("Count").toInt();  //获取工作表数目
        }
        int sheetindex = 0;
        while(sheetindex<sheetCount)
        {
            QAxObject *worksheet = workbook->querySubObject("Sheets(int)", sheetindex+1);//获取第i个工作表
            if(worksheet == nullptr)
            {
                info.append("获取子表失败："+str);
                return {};
            }
            QAxObject *usedRange = worksheet->querySubObject("UsedRange");
            if(nullptr == usedRange || usedRange->isNull())
            {
                info.append("获取第"+QString::number(sheetindex+1)+"个子表时失败（"+str+")");
                return {};
            }
            QVariant var = usedRange->dynamicCall("Value");
            delete usedRange;
            sheets.push_back(castVariant2ListListVariant(var));
            sheetindex++;
        }
        m.push_back(QPair<FileInfo, ExcelFile>(FI,qMove(sheets)));
    }
    return m;
}

Excel2QStr::~Excel2QStr()
{
	workbooks->dynamicCall("Close(Boolean)", false);
	excel.dynamicCall("Quit(void)");
	delete workbooks;
}


