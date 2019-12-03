#include <QFileDialog>
#include <QDebug>
#include <QString>
#include<ActiveQt/QAxObject>
#include <QColor>
#include <QMessageBox>
#include <QMetaType>
#include "choosedesignfiles_dialog.h"
#include "ui_choosedesignfiles.h"


bool ChooseDesignFiles_DIALOG::importAgain = false;

ChooseDesignFiles_DIALOG ::ChooseDesignFiles_DIALOG (QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChooseDesignFiles_DIALOG )
{
    ui->setupUi(this);
    jinluNum = 0;
    chezhanNum = 0;
    poduNum = 0;
    guofenxiangNum = 0;
    shujuNum = 0;
    suduNum = 0;
    yingdaqiNum = 0;
    duanlianNum = 0;
    zuobiaoxiNum = 0;
    lichengNum = 0;
    zhantaiceNum = 0;
    connect(tcpCommandClient, &TcpCommandClient::HasReturn2Sheet_SIGNAL, this, &ChooseDesignFiles_DIALOG::handleResult);
}

ChooseDesignFiles_DIALOG ::~ChooseDesignFiles_DIALOG ()
{
    qDebug() << "dialog析构被调用";
    delete ui;
}


void ChooseDesignFiles_DIALOG::on_pushButton_clicked()
{
    static int diaoyongcishu = 1;
    importAgain = false;
    if(diaoyongcishu > 1)
    {
        importAgain = true;
    }

	//清理工作
	jinluNum = 0;
	chezhanNum = 0;
	poduNum = 0;
	guofenxiangNum = 0;
	shujuNum = 0;
	suduNum = 0;
	yingdaqiNum = 0;
	duanlianNum = 0;
	zuobiaoxiNum = 0;
	lichengNum = 0;
	zhantaiceNum = 0;
	DesignData::clearAll();

    ui->logListWidget->clear();
    ui->progressBar->setValue(0);
    readFileThread = new ReadFileThread(this);

    fileNameList = QFileDialog::getOpenFileNames(
                this, "导入列控数据", "", "EXCEL (*.xls *.xlsx *.dat)");
	if (fileNameList.size() == 1 && fileNameList[0].endsWith(".dat"))
	{
		QByteArray QBA;
		QFile sheetFile(fileNameList[0]);
		if (sheetFile.open(QIODevice::ReadOnly))
		{
			QBA = sheetFile.readAll();
			tcpCommandClient->send(Combine_Command_Data(TcpHead(CMD_FROM::CLIENT, CMD_TYPE::CONTROL, CMD_NAME::DATA_SHEET), QBA));
			sheetFile.close();
			handleResult("数据已发送");
		}
		return;
	}
    readFileThread->filePath = fileNameList;
    qRegisterMetaType<FileType>("FileType");
    connect(readFileThread, &ReadFileThread::showProgressBar,
            this, &ChooseDesignFiles_DIALOG::handleProgressBar);
    connect(readFileThread, &ReadFileThread::showResult,
            this, &ChooseDesignFiles_DIALOG::handleResult);
    connect(readFileThread, &ReadFileThread::addNumSignal,
            this, &ChooseDesignFiles_DIALOG::addFileNum);
    connect(readFileThread, &ReadFileThread::workfinished_SIGNAL,
                this, &ChooseDesignFiles_DIALOG::deleteThread);

    ui->finishPushButton->setEnabled(false);
    ui->pushButton->setEnabled(false);


    readFileThread->start();
}

void ChooseDesignFiles_DIALOG::handleProgressBar(int value)
{
    ui->progressBar->setValue(value);

}

void ChooseDesignFiles_DIALOG::handleResult(QString txt)
{
   QListWidgetItem *item = new QListWidgetItem(txt);
   QBrush *brush = new QBrush(Qt::red);
   if(txt.contains("失败"))
   {
      item->setForeground(*brush);
   }
   ui->logListWidget->addItem(item);
   ui->logListWidget->scrollToBottom();
}

void ChooseDesignFiles_DIALOG::deleteThread()
{
	ui->finishPushButton->setEnabled(true);
	ui->pushButton->setText("重新导入");
	ui->pushButton->setEnabled(true);
    qDebug()<<"deleteThread";
    readFileThread->deleteLater();
    readFileThread = NULL;
//    ui->escPushButton->setEnabled(false);
}


void ChooseDesignFiles_DIALOG::closeEvent(QCloseEvent *event)
{
    if(readFileThread)
    {
        QMessageBox::warning(this, "警告", "文件正在被读取，请耐心等待", QMessageBox::Yes);
        event->ignore();  //忽略退出信号
    }
    else
    {
        event->accept();
    }
}

void ChooseDesignFiles_DIALOG::addFileNum(FileType fileType)
{
    qDebug()<<"addfile";
    switch(fileType)
    {
        case JINLU:
            jinluNum++;
            break;
        case CHEZHAN:
            chezhanNum++;
            break;
        case PODU:
            poduNum++;
            break;
        case FENXIANG:
            guofenxiangNum++;
            break;
        case XIANLUSHUJU:
            shujuNum++;
            break;
        case SUDU:
            suduNum++;
            break;
        case YINGDAQIWEIZHI:
            yingdaqiNum++;
            break;
        case DUANLIAN:
            duanlianNum++;
            break;
        case ZHANTAI:
            zhantaiceNum++;
            break;
        default:
            break;
    }
}

void ChooseDesignFiles_DIALOG::on_finishPushButton_clicked()
{
    QString str = "";
	QString returnStr = "";

    if(jinluNum == 0)
    {
        str += "进路数据表  ";
    }
    if(chezhanNum == 0)
    {
        str += "车站信息表  ";
    }
    if(poduNum == 0)
    {
        str += "坡度信息表  ";
    }
    if(guofenxiangNum == 0)
    {
        str += "分相区信息表  ";
    }
    if(shujuNum == 0)
    {
        str += "线路数据表  ";
    }
    if(suduNum == 0)
    {
        str += "线路速度表  ";
    }
    if(yingdaqiNum == 0)
    {
        str += "线路速度表  ";
    }
    if(duanlianNum == 0)
    {
        str += "短链明细表  ";
    }
    if(zhantaiceNum == 0)
    {
        str += "站台侧信息表  ";
    }

	sheetMutex.lock();
    foreach(auto var,sRecordVec)
	{
		if (!var.isReturn)
		{
			returnStr += (var.sheetName + "  ");
		}
	}
	sheetMutex.unlock();

    if(!str.isEmpty())
    {
        str += "尚未导入\n";
		returnStr += "确定继续？";
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, tr("提示"),
                                           str,
                                           QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes)
        {
               delete this;
        }
    }
    else
    {
        delete this;
    }
}
