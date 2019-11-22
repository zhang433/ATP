#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QFile>
#include <QString>
#include <QDebug>
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
	ui->textBrowser->append("->"+info);
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
    QStringList fileNameList = QFileDialog::getOpenFileNames(
                this, "导入列控数据", "", "EXCEL (*.xls *.xlsx)");
    ui->textBrowser->clear();
    QString info;
    Excel2QStr E2Q(fileNameList,info);
    auto ConvertFiles = E2Q.getQStrTable();
    if(!info.isEmpty())
    {
        ui->textBrowser->append(info);
        return;
    }
	QList<QPair<QString, Sheet>> All_ConvertSheet;
	if(!Rules_CHEZHAN::convert(ConvertFiles,All_ConvertSheet))
		return;
    if(!Rules_GUOFENXIANG::convert(ConvertFiles,All_ConvertSheet))
        return;
    if(!Rules_JINLU::convert(ConvertFiles,All_ConvertSheet))
        return;
    if(!Rules_PODU::convert(ConvertFiles,All_ConvertSheet))
        return;
    if(!Rules_SUDU::convert(ConvertFiles,All_ConvertSheet))
        return;
    if(!Rules_XIANLUSHUJU::convert(ConvertFiles,All_ConvertSheet))
        return;
    if(!Rules_YINGDAQIWEIZHI::convert(ConvertFiles,All_ConvertSheet))
        return;
    if(!Rules_ZHANTAI::convert(ConvertFiles,All_ConvertSheet))
        return;
    if(!Rules_DUANLIAN::convert(ConvertFiles,All_ConvertSheet))
        return;
    if(!ConvertFiles.isEmpty())
    {
        printinfo("无关文件：");
        for(auto& x:ConvertFiles)
        {
            printinfo(x.first.fileName);
        }
    }
	printinfo("已完成");
//    for(auto& pair:All_ConvertSheet)
//    {
//        qDebug()<<pair;
//        QFile file;
//        QString path = fileNameList[0];
//        int index = path.lastIndexOf('\\');
//        if(index==-1)
//            index = path.lastIndexOf('/');
//        file.setFileName(path.mid(0,index+1)+pair.first+".csv");
//        file.open(QIODevice::WriteOnly);
//        assert(file.isOpen());
//        file.write()
//    }
    return;
}
