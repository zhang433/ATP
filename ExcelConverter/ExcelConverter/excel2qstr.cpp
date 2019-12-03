#include "excel2qstr.h"
#include <QString>
#include <QDebug>
#include <QtAlgorithms>
const QMap<QString,FileType> Excel2QStr::KeyWord_FileName = {{"进路",FileType::JINLU},{"车站",FileType::CHEZHAN},{"坡度",FileType::PODU},{"分相",FileType::FENXIANG},{"线路数据",FileType::XIANLUSHUJU},{"速度",FileType::SUDU},{"应答器位置",FileType::YINGDAQIWEIZHI},{"断链",FileType::DUANLIAN},{"站台",FileType::ZHANTAI}};

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
        ret.second.push_back({});
        for(int j=0;j<res[i].size();++j)
        {
            ret.second.back().push_back(res[i][j].toString());
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
        FI.fileType = FileType::NONE;
        for(auto iter = KeyWord_FileName.begin();iter!=KeyWord_FileName.end();++iter)
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
            QString sheetName = worksheet->property("Name").toString();
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
            auto sheet = castVariant2ListListVariant(var);
            if(sheetName.contains("上"))
            {
                if(sheetName.contains("正"))
                    sheet.first = SheetType::UP_FRONT;
                else if(sheetName.contains("反"))
                    sheet.first = SheetType::UP_BACK;
                else
                    sheet.first = SheetType::UP;
            }
            else if(sheetName.contains("下"))
            {
                if(sheetName.contains("正"))
                    sheet.first = SheetType::DOWN_FRONT;
                else if(sheetName.contains("反"))
                    sheet.first = SheetType::DOWN_BACK;
                else
                    sheet.first = SheetType::DOWN;
            }
            else
                sheet.first = SheetType::UNKNOWN;
            sheets.push_back(sheet);
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


