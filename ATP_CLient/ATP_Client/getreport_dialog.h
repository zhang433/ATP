#ifndef GETREPORT_DIALOG_H
#define GETREPORT_DIALOG_H
#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

#include <QMainWindow>
#include <QLabel>
#include <QMessageBox>
#include <QRect>
#include <QColor>
#include <QTreeWidgetItem>
#include "Command.h"
#include "Excelwork.h"
#include "basestructer.h"
#include "analyze_resmessage.h"

#define xlLeft -4131 //左对齐
#define xlCenter -4108 //居中
#define xlRight -4152


namespace Ui {
class GetReport_DIALOG;
}

class GetReport_DIALOG : public QMainWindow
{
    Q_OBJECT

public:
    explicit GetReport_DIALOG(QWidget *parent = nullptr);
    ~GetReport_DIALOG();

signals:
    void InitSheets(QStringList nameList);
    void SaveAs(QString filePath);
    void WriteData(int worksheetIndex, QRect rect, QVector<QVector<QString>> data, QColor color);
    void CombineCell(int worksheetIndex, QRect combineRect, int horizontalAlignment);
	void AddHyperlink(int anchorSheetIndex, QRect anchor, int addSheetIndex, QRect address);
	void ProgressStep(QString log, int progressValue);
private slots:
    void on_pushButton_GetFileList_clicked();

	void on_pushButton_Download_clicked();

	void on_pushButton_Delete_clicked();

    void GetFileList_SLOT(QVector<QVector<QString>>);

	void GetDownloadFile_SLOT(QVector<QString>, QVector<QByteArray>);

    void GetSelectedFile_SLOT(QVector<SequenceDataStructure> V_SDS);

    void on_pushButton_GetSelectedFile_clicked();

    void on_treeWidget_FIleList_clicked(const QModelIndex &index);

    void on_treeWidget_FIleList_itemClicked(QTreeWidgetItem *item, int column);

    void AddProgressAndLog(QString log, int progressValue);

	void errorHandler(int errorCode);

private:
    QVector<int> ErrorFilter(const SequenceDataStructure &sds, CompareType ct);
    QVector<QVector<QVector<QString> > > GetErrorBaliseData(SequenceDataStructure &sds, QVector<int> errorRecord, CompareType ct, QVector<QString> &baliseIDVec);
    QVector<QVector<QVector<QString> > > GetErrorCompareResult(const SequenceDataStructure &sds, QVector<int> errorRecord, CompareType ct);
    QString generateFilePath;//报表文件的生成路径
	QString generateDirPath;
    Ui::GetReport_DIALOG *ui;
    QLabel *msgLabel;//用于显示底部状态栏连接信息
	QThread excelTh;
    Excelwork* excelWork;
    QString filePath;
    QVector<QVector<QString> > FileList;
private slots:
    void updateStatus(QString color,QString content);
    void on_comboBox_currentIndexChanged(const QString &arg1);
};
extern GetReport_DIALOG* GR_DIALOG;

#endif // GETREPORT_DIALOG_H
