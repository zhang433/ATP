/*
此类提供QT对EXCEL操作的接口
*/
#pragma once
#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

#include <QObject>
#include <qaxobject.h>
#include <qvector.h>
#include <qstringlist.h>
#include <qrect.h>
#include <qpoint.h>
#include <QColor>

#define xlLeft -4131 //左对齐
#define xlCenter -4108 //居中
#define xlRight -4152

class Excelwork : public QObject
{
	Q_OBJECT

public:
	Excelwork(QObject *parent);
	~Excelwork();
signals:
	void errorInfo(int errorCode); //错误码0， 打开excel失败  
	void progressFeedBack(QString log, int progressValue);
    void finished();
public slots:
	void InitSheets(QStringList nameList); //建立主表和子表并填写表
    void WriteSheet(int worksheetIndex, QRect rect, QVector<QVector<QString>> data, QColor color);// //向指定表的指定位置插入表格,并指定背景颜色
    QRect CombineCell(int worksheetIndex, QRect combineRect, int horizontalAlignment);   //合并指定表指定范围内单元格
	void AddHyperlink(int anchorSheetIndex, QRect anchor, int addSheetIndex, QRect address);
	void saveAs(QString filePath);
	void ProgressDown(QString log, int progressValue);

private:
	QAxObject * excel;
	QAxObject *workbooks;   //工作簿集合
	QAxObject *workbook;    //工作簿
	QAxObject *worksheets;  //工作表集合
	QVector<QAxObject *> worksheetVec; //工作表
	QVector<QPoint> worksheetSizeVec; //各工作表大小
	QStringList nameList;
};
