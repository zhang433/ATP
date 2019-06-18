#include "Excelwork.h"
#include <QProcess>
#include <ActiveQt/QAxObject>
#include <QDebug>
#include <qt_windows.h>
#include <qthread.h>
#include <qdir.h>


Excelwork::Excelwork(QObject *parent)
	: QObject(parent)
{
    excel = nullptr;
    workbooks = nullptr;
    workbook = nullptr;
    worksheets = nullptr;
}

Excelwork::~Excelwork()
{
}

void Excelwork::WriteSheet(int worksheetIndex, QRect rect, QVector<QVector<QString>> data, QColor color)//保存所有行数据)
{
	QList<QVariant> allRowsData;//保存所有行数据
	for (int row = 0; row < data.size(); row++)
	{
		QList<QVariant> aRowData;//保存一行数据
		for (int column = 0; column < data[row].size(); column++)
		{
			aRowData.append(QVariant(data[row][column]));
		}
		allRowsData.append(QVariant(aRowData));
	}
	QString range_s;
	range_s.append(QChar(rect.left() - 1 + 'A'));
	range_s.append(QString::number(rect.top()));  //初始行 
	range_s.append(":");
	range_s.append(QChar(rect.right() - 1 + 'A'));  //终止列 
	range_s.append(QString::number(rect.bottom()));  //终止行 
	QAxObject *range = worksheetVec[worksheetIndex]->querySubObject("Range(const QString )", range_s);
	range->dynamicCall("SetValue(const QVariant&)", QVariant(allRowsData));//存储所有数据到 excel 中,批量操作,速度极快
	range->setProperty("WrapText", true);
    if (color != Qt::white)
	{
		QAxObject* interior = range->querySubObject("Interior");
		interior->setProperty("Color", color);   //设置单元格背景色（绿色）
	}

}

QRect Excelwork::CombineCell(int worksheetIndex, QRect combineRect, int horizontalAlignment)
{
	QString merge_cell;
	merge_cell.append(QChar(combineRect.left() - 1 + 'A'));  //初始列 
	merge_cell.append(QString::number(combineRect.top()));  //初始行 
	merge_cell.append(":");
	merge_cell.append(QChar(combineRect.right() - 1 + 'A'));  //终止列 
	merge_cell.append(QString::number(combineRect.bottom()));  //终止行 
	QAxObject *merge_range = worksheetVec[worksheetIndex]->querySubObject("Range(const QString&)", merge_cell);
	merge_range->setProperty("HorizontalAlignment", horizontalAlignment);
	merge_range->setProperty("VerticalAlignment", xlCenter);
	merge_range->setProperty("WrapText", true);
	merge_range->setProperty("MergeCells", true);  //合并单元格
	return combineRect;
}

void Excelwork::AddHyperlink(int anchorSheetIndex, QRect anchor, int addSheetIndex, QRect address)
{
	QString anchor_cell;
	anchor_cell.append(QChar(anchor.left() - 1 + 'A'));  //初始列 
	anchor_cell.append(QString::number(anchor.top()));  //初始行 
	anchor_cell.append(":");
	anchor_cell.append(QChar(anchor.right() - 1 + 'A'));  //终止列 
	anchor_cell.append(QString::number(anchor.bottom()));  //终止行 

	QString address_cell;
	address_cell.append(QChar(address.left() - 1 + 'A'));  //初始列 
	address_cell.append(QString::number(address.top()));  //初始行 
	//address_cell.append(":");
	//address_cell.append(QChar(address.right() - 1 + 'A'));  //终止列 
	//address_cell.append(QString::number(address.bottom()));  //终止行
	
	address_cell = QString("%1!%2").arg(nameList[addSheetIndex]).arg(address_cell);
	QAxObject *anchor_range = worksheetVec[anchorSheetIndex]->querySubObject("Range(const QString&)", anchor_cell);
	/*QAxObject *address_range = worksheetVec[addSheetIndex]->querySubObject("Range(const QString&)", address_cell);
	*/
	QAxObject *hyperlinks = worksheetVec[anchorSheetIndex]->querySubObject("Hyperlinks");
	hyperlinks->dynamicCall("Add(const QVariant&, const QString&, const QString&)", anchor_range->asVariant(), "", address_cell);
}



void Excelwork::saveAs(QString filePath)
{
	qDebug() << "STH:" << QThread::currentThreadId();
	workbook->dynamicCall("SaveAs(const QString&)", QDir::toNativeSeparators(filePath));//保存至filepath，注意一定要用QDir::toNativeSeparators将路径中的"/"转换为"\"，不然一定保存不了。
	workbook->dynamicCall("Close(Boolean)", false);//关闭工作簿
	excel->dynamicCall("Quit(void)");//关闭excel
	OleUninitialize();
    emit finished();
}

void Excelwork::ProgressDown(QString log, int progressValue)
{
	static int pv = 0;
	if (progressValue == 100)
	{
		pv = 100;
	}
	else
	{
		pv += progressValue;
	}
	progressFeedBack(log, pv);
}

void Excelwork::InitSheets(QStringList nameList)
{

	//CoInitializeEx(NULL, COINIT_MULTITHREADED);
	/*----------------------创建EXCEL----------------------------------------------*/
	//在子线程中使用COM组件需手动初始化
	HRESULT r = OleInitialize(0);
	if (r != S_OK && r != S_FALSE) {
		qWarning("Qt: Could not initialize OLE (error %x)", (unsigned int)r);
	}


	excel = new QAxObject("Excel.Application");
    if (excel == nullptr)
	{
		qDebug() << "打开EXCEL失败";
		emit errorInfo(0);
		//emit showResult("打开EXCEL失败，请检查EXCEL是否安装或激活！");
		return;
	}
	excel->setProperty("Visible", false); //不显示窗体
	excel->setProperty("DisplayAlerts", false);//不显示提示信息

	this->nameList = nameList;										  //
	workbooks = excel->querySubObject("WorkBooks");//获取工作簿集合
	workbooks->dynamicCall("Add");//新建一个工作簿
	workbook = excel->querySubObject("ActiveWorkBook");//获取当前工作簿
	worksheets = workbook->querySubObject("Sheets");//获取工作表集合

	worksheetVec.resize(nameList.size());
	worksheetSizeVec.resize(nameList.size());

	worksheetVec[0] = worksheets->querySubObject("Item(int)", 1);//获取工作表集合的工作表1，即sheet1
	worksheetVec[0]->setProperty("Name", nameList[0]);
	for (int i = 1; i < nameList.size(); ++i)
	{
		int sheet_count = worksheets->property("Count").toInt();  //获取工作表数目
		QAxObject *last_sheet = worksheets->querySubObject("Item(int)", sheet_count);
		QAxObject *work_sheet = worksheets->querySubObject("Add(QVariant)", last_sheet->asVariant());
		last_sheet->dynamicCall("Move(QVariant)", work_sheet->asVariant());

		work_sheet->setProperty("Name", nameList[i]);  //设置工作表名称
		worksheetVec[i] = worksheets->querySubObject("Item(int)", i + 1);
	}


	worksheetSizeVec[0] = QPoint(0, nameList.size() + 2);  //设置表1大小

														   //处理总表的表头内容
	QVector<QVector<QString> > res;
	int row = 1;
	int col = 1;
	res.resize(row);//r行
	for (int k = 0; k < row; ++k) {
		res[k].resize(col);//每行为c列
	}
	res[0][0] = "车载监测设备分析报表";
	CombineCell(0, QRect(1, 1, worksheetSizeVec[0].y(), 1), xlCenter);
	WriteSheet(0, QRect(1, 1, 1, 1), res, Qt::green);

	res[0][0] = "试验日期";
    CombineCell(0, QRect(2, 2, worksheetSizeVec[0].y() - 1, 1), xlLeft);
    WriteSheet(0, QRect(1, 2, 1, 1), res, Qt::white);

	res[0][0] = "起止时间";
    CombineCell(0, QRect(2, 3, worksheetSizeVec[0].y() - 1, 1), xlLeft);
    WriteSheet(0, QRect(1, 3, 1, 1), res, Qt::white);

	res[0][0] = "完成序列数";
    CombineCell(0, QRect(2, 4, worksheetSizeVec[0].y() - 1, 1), xlLeft);
    WriteSheet(0, QRect(1, 4, 1, 1), res, Qt::white);

	res[0][0] = "检测里程";
    CombineCell(0, QRect(2, 5, worksheetSizeVec[0].y() - 1, 1), xlLeft);
    WriteSheet(0, QRect(1, 5, 1, 1), res, Qt::white);

    res[0][0] = "序号     ";
	CombineCell(0, QRect(1, 7, 1, 2), xlLeft);
	WriteSheet(0, QRect(1, 7, 1, 1), res, Qt::green);

    res[0][0] = "试验序列号";
	CombineCell(0, QRect(2, 7, 1, 2), xlLeft);
	WriteSheet(0, QRect(2, 7, 1, 1), res, Qt::green);

	res[0][0] = "时间";
	CombineCell(0, QRect(3, 7, 1, 2), xlLeft);
	WriteSheet(0, QRect(3, 7, 1, 1), res, Qt::green);

	res[0][0] = "问题统计";
	CombineCell(0, QRect(4, 7, worksheetSizeVec[0].y() - 3, 1), xlLeft);
	WriteSheet(0, QRect(4, 7, 1, 1), res, Qt::green);

	for (int i = 0; i < nameList.size() - 1; i++)
	{
		res[0][0] = nameList[i + 1];
	    WriteSheet(0, QRect(4 + i, 8, 1, 1), res, Qt::green);
	}
}
