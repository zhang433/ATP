/*
�����ṩQT��EXCEL�����Ľӿ�
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

#define xlLeft -4131 //�����
#define xlCenter -4108 //����
#define xlRight -4152

class Excelwork : public QObject
{
	Q_OBJECT

public:
	Excelwork(QObject *parent);
	~Excelwork();
signals:
	void errorInfo(int errorCode); //������0�� ��excelʧ��  
	void progressFeedBack(QString log, int progressValue);
    void finished();
public slots:
	void InitSheets(QStringList nameList); //����������ӱ���д��
    void WriteSheet(int worksheetIndex, QRect rect, QVector<QVector<QString>> data, QColor color);// //��ָ�����ָ��λ�ò�����,��ָ��������ɫ
    QRect CombineCell(int worksheetIndex, QRect combineRect, int horizontalAlignment);   //�ϲ�ָ����ָ����Χ�ڵ�Ԫ��
	void AddHyperlink(int anchorSheetIndex, QRect anchor, int addSheetIndex, QRect address);
	void saveAs(QString filePath);
	void ProgressDown(QString log, int progressValue);

private:
	QAxObject * excel;
	QAxObject *workbooks;   //����������
	QAxObject *workbook;    //������
	QAxObject *worksheets;  //��������
	QVector<QAxObject *> worksheetVec; //������
	QVector<QPoint> worksheetSizeVec; //���������С
	QStringList nameList;
};
