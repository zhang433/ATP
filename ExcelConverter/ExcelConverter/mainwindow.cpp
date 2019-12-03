#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QFile>
#include <QString>
#include <QDebug>
#include <QTextBrowser>
#include <iostream>
#include "excel2qstr.h"
#include "rules_yingdaqiweizhi.h"
#include "rules_chezhan.h"
#include "rules_guofenxiang.h"
#include "rules_jinlu.h"
#include "rules_podu.h"
#include "rules_sudu.h"
#include "rules_xianlushuju.h"
#include "rules_zhantai.h"
#include "rules_duanlian.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{ 
    ui->setupUi(this);
    ui->textBrowser->append("表格命名规范：");
    ui->textBrowser->append("进路数据表    -   *进路*.xls/xlsx");
    ui->textBrowser->append("车站信息表    -   *车站*.xls/xlsx");
    ui->textBrowser->append("线路坡度表    -   *坡度*.xls/xlsx");
    ui->textBrowser->append("分相区信息表   -   *分相*.xls/xlsx");
    ui->textBrowser->append("线路数据表    -   *线路数据*.xls/xlsx");
    ui->textBrowser->append("线路速度表    -   *速度*.xls/xlsx");
    ui->textBrowser->append("应答器位置表   -   *应答器位置*.xls/xlsx");
    ui->textBrowser->append("里程断链明细表  -   *断链*.xls/xlsx");
    ui->textBrowser->append("站台侧信息表   -   *站台*.xls/xlsx");
}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::printinfo(QString info)
{
	if (Print2Screen)
		ui->textBrowser->append("->" + info);
	else
		InnerInfo.append("\n->" + info);
}

void MainWindow::printerr(QString err,QString line,QString extra)
{
	if (!err.isEmpty())
		printinfo("错误原因:" + err);
	if(!line.isEmpty())
        printinfo("出错位置:" + line);
	if(!extra.isEmpty())
		printinfo("说明:" + extra);
}


void MainWindow::on_pushButton_clicked()
{
	Print2Screen = true;
    fileNameList = QFileDialog::getOpenFileNames(
                this, "导入列控数据", "", "EXCEL (*.xls *.xlsx)");
    QByteArray QBA;
    QDataStream QDS(&QBA, QIODevice::WriteOnly);
    QDS.setVersion(QDataStream::Qt_5_12);
    //Check
    if (!ConvertFile())
    {
        QDS << QString("ERROR") << ui->textBrowser->toPlainText();
    }
    else
    {
        QDS << QString("OK") << DesignData::accessRodeMap << DesignData::stationVec << DesignData::gradeDownProVec << DesignData::gradeUpProVec\
            << DesignData::gradeDownBackVec << DesignData::gradeUpBackVec << DesignData::neutralSectionUpVec << DesignData::neutralSectionDownVec\
            << DesignData::pathWayDataDownProVec << DesignData::pathWayDataUpProVec << DesignData::pathWayDataDownBackVec << DesignData::pathWayDataUpBackVec\
            << DesignData::pathWaySpeedDownVec << DesignData::pathWaySpeedUpVec << DesignData::pathWaySpeedDownBackVec << DesignData::pathWaySpeedUpBackVec\
            << DesignData::baliseLocationUpMap << DesignData::baliseLocationDownMap << DesignData::balishUseMap << DesignData::brokenLinkVec << DesignData::stationSideVec;
    }
    QFile dataSheet(QApplication::applicationDirPath() + '/' + "sheet.datastream");
    if (dataSheet.open(QIODevice::WriteOnly))
    {
        dataSheet.write(QBA);
        dataSheet.close();
    }

    /*********************************/
    QFile dataSheet1(QApplication::applicationDirPath() + '/' + "sheet.datastream");
    dataSheet1.open(QIODevice::ReadOnly);
    QByteArray QBA1 = dataSheet1.readAll();
    QString result;
    QDataStream QDS1(&QBA1, QIODevice::ReadOnly);
    QDS1.setVersion(QDataStream::Qt_5_12);
    QDS1>>result;
    qDebug()<<result;
    QDS1>>result;
    qDebug()<<result;
    /***********************************/
}

bool MainWindow::ConvertFile()
{
    ui->textBrowser->clear();
    QString info;
    QList<QPair<FileInfo, ExcelFile>> ConvertFiles;
    {
        Excel2QStr E2Q(fileNameList,info);
        ConvertFiles = E2Q.getQStrTable();
    }
    if(!info.isEmpty())
    {
        ui->textBrowser->append(info);
        return false;
    }
    All_ConvertSheet.clear();
    if(!Rules_CHEZHAN::convert(ConvertFiles,All_ConvertSheet))
        return false;
    if(!Rules_GUOFENXIANG::convert(ConvertFiles,All_ConvertSheet))
        return false;
    if(!Rules_JINLU::convert(ConvertFiles,All_ConvertSheet))
        return false;
    if(!Rules_PODU::convert(ConvertFiles,All_ConvertSheet))
        return false;
    if(!Rules_SUDU::convert(ConvertFiles,All_ConvertSheet))
        return false;
    if(!Rules_XIANLUSHUJU::convert(ConvertFiles,All_ConvertSheet))
        return false;
    if(!Rules_YINGDAQIWEIZHI::convert(ConvertFiles,All_ConvertSheet))
        return false;
    if(!Rules_ZHANTAI::convert(ConvertFiles,All_ConvertSheet))
        return false;
    if(!Rules_DUANLIAN::convert(ConvertFiles,All_ConvertSheet))
        return false;
    if(!ConvertFiles.isEmpty())
    {
        printinfo("无关文件：");
        for(auto& x:ConvertFiles)
        {
            printinfo(x.first.fileName);
        }
    }
    printinfo("已完成");
    qDebug()<<ui->textBrowser->toPlainText();
    return true;
}
