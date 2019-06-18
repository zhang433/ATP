#include "getreport_dialog.h"
#include "ui_getreport_dialog.h"
#include "tcpcommandclient.h"
#include <QFileDialog>
#include <qthread.h>
#include <QDateTime>

GetReport_DIALOG* GR_DIALOG;

GetReport_DIALOG::GetReport_DIALOG(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GetReport_DIALOG)
{
    ui->setupUi(this);
    msgLabel = new QLabel(this);
    msgLabel->setStyleSheet(" QLabel{ color: red }");
    msgLabel->setText("");
    ui->statusbar->addWidget(msgLabel);
    ui->pushButton_GetSelectedFile->setCheckable(false);

    ui->treeWidget_FIleList->setSelectionMode(QAbstractItemView::SelectionMode::MultiSelection);
    ui->progressBar->setRange(0, 100);
    ui->progressBar->setValue(0);

    connect(tcpCommandClient,SIGNAL(GetFileList_SIGNAL(QVector<QVector<QString>>)),this,SLOT(GetFileList_SLOT(QVector<QVector<QString>>)));
    connect(tcpCommandClient,SIGNAL(GetSelectedFile_SIGNAL(QVector<SequenceDataStructure>)),this,SLOT(GetSelectedFile_SLOT(QVector<SequenceDataStructure>)));
	connect(tcpCommandClient, SIGNAL(GetDownloadFile_SIGNAL(QVector<QString>,QVector<QByteArray>)), this, SLOT(GetDownloadFile_SLOT(QVector<QString>, QVector<QByteArray>)));
    connect(tcpCommandClient, SIGNAL(changeDialogStatue_SIGNAL(QString, QString)), this, SLOT(updateStatus(QString, QString)));
}

GetReport_DIALOG::~GetReport_DIALOG()
{
    delete ui;
}

void GetReport_DIALOG::on_pushButton_GetFileList_clicked()
{
    tcpCommandClient->send(Combine_Command_Data(TcpHead(CMD_FROM::CLIENT,CMD_TYPE::CONTROL,CMD_NAME::LIST_FILE),QString("/mnt/ReportFile")));
    updateStatus("red","正在等待工控机返回文件列表信息...");
}

void GetReport_DIALOG::on_pushButton_Download_clicked()
{
	QList<QTreeWidgetItem*> selectedItemList = ui->treeWidget_FIleList->selectedItems();
	QVector<QString> v_str;
	QString arg = "download";
	for (auto& selectedItem : selectedItemList)
		if (selectedItem->parent() != nullptr)
			v_str.push_back(selectedItem->parent()->text(0) + "/" + selectedItem->text(0));
	if (v_str.isEmpty())//前提条件判定
	{
		QMessageBox::information(this, "提示", "没有选择下载的文件");
		return;
	}
	generateDirPath = QFileDialog::getExistingDirectory(this, "请选择报表生成路径", QCoreApplication::applicationDirPath());
	if (generateDirPath.isEmpty())
		return;
	tcpCommandClient->send(Combine_Command_Data(TcpHead(CMD_FROM::CLIENT, CMD_TYPE::CONTROL, CMD_NAME::GET_SELECTED_FILE), arg, v_str));
}

void GetReport_DIALOG::on_pushButton_Delete_clicked()
{
	QList<QTreeWidgetItem*> selectedItemList = ui->treeWidget_FIleList->selectedItems();
	QVector<QString> v_str;
	QString arg = "delete";
	for (auto& selectedItem : selectedItemList)
	{
		if(selectedItem->parent()!=nullptr)
			v_str.push_back(selectedItem->parent()->text(0) + "/" + selectedItem->text(0));
	}
	if (v_str.isEmpty())//前提条件判定
	{
		QMessageBox::information(this, "提示", "没有选择删除的文件");
		return;
	}
	tcpCommandClient->send(Combine_Command_Data(TcpHead(CMD_FROM::CLIENT, CMD_TYPE::CONTROL, CMD_NAME::GET_SELECTED_FILE), arg, v_str));
}

void GetReport_DIALOG::GetFileList_SLOT(QVector<QVector<QString> > FileList)
{
    this->FileList = FileList;
    on_comboBox_currentIndexChanged(ui->comboBox->currentText());
    updateStatus("green","已更新文件列表");
}

void GetReport_DIALOG::GetDownloadFile_SLOT(QVector<QString> fileName, QVector<QByteArray> binData)
{
	assert(fileName.size() == binData.size());
	AddProgressAndLog("正在写入文件...",0);
	int error_count = 0;
	for (int i = 0; i < fileName.size(); ++i)
	{
		if (fileName[i].contains('/'))
		{
			QStringList QSL = fileName[i].split('/');
			QString dirPath = generateDirPath;
			for (int i = 0; i < QSL.size() - 1; ++i)
			{
				dirPath = dirPath + "/" + QSL[i];
				QDir dir(dirPath);
				if (!dir.exists())
					dir.mkdir(dirPath);
			}
		}
		QFile file(generateDirPath + '/' + fileName[i]);
		if (file.open(QIODevice::WriteOnly))
		{
			file.write(binData[i]);
			file.close();
		}
		else
		{
			qDebug() << file.error() << file.errorString();
			error_count++;
		}
		AddProgressAndLog("正在写入" + fileName[i], (100.0 / fileName.size()) * i);
	}
	if (error_count == 0)
		AddProgressAndLog("全部文件写入完成", 100);
	else
		AddProgressAndLog(QString::number(error_count) + "个文件写入失败", 100);
}

void GetReport_DIALOG::GetSelectedFile_SLOT(QVector<SequenceDataStructure> V_SDS)
{
	updateStatus("red", "正在处理数据内容...");

	//qDebug() << "MTH:" << QThread::currentThreadId();
	QStringList nameList;
	nameList << "总表" << "应答器位置" << "轨道电路" << "线路速度" << "线路坡度" << "车站名与站台侧" << "分相区";

	excelWork = new Excelwork(nullptr);
	excelWork->moveToThread(&excelTh);
	connect(excelWork, &Excelwork::finished, excelWork, &Excelwork::deleteLater);//workfinished_SIGNAL
	connect(this, &GetReport_DIALOG::InitSheets, excelWork, &Excelwork::InitSheets);
	connect(this, &GetReport_DIALOG::SaveAs, excelWork, &Excelwork::saveAs);
	connect(this, &GetReport_DIALOG::WriteData, excelWork, &Excelwork::WriteSheet);
	connect(this, &GetReport_DIALOG::CombineCell, excelWork, &Excelwork::CombineCell);
	connect(this, &GetReport_DIALOG::AddHyperlink, excelWork, &Excelwork::AddHyperlink);
	connect(this, &GetReport_DIALOG::ProgressStep, excelWork, &Excelwork::ProgressDown);
	connect(excelWork, &Excelwork::progressFeedBack, this, &GetReport_DIALOG::AddProgressAndLog);
	excelTh.start();

	emit InitSheets(nameList);

	//开始处理数据内容
	QVector<QVector<QString> > res;
	int row = 1;
	int col = 1;
	res.resize(row);//r行
	for (int k = 0; k < row; ++k) {
		res[k].resize(col);//每行为c列
	}

	//检测V_SDS中第一个和最后一个reportData非空的位置，计算时间
	int firstPos = -1;
	int lastPos = -1;
	for (int i = 0; i < V_SDS.size(); i++)
	{
		if (!V_SDS[i].reportData.isEmpty())
		{
			if (firstPos == -1)
			{
				firstPos = i;
			}
			lastPos = i;
		}
	}

	if (firstPos != -1 && lastPos != -1)
	{
		QDateTime time_start = QDateTime::fromTime_t(V_SDS[firstPos].reportData.front().ATPTimestamp);

		QDateTime time_end = QDateTime::fromTime_t(V_SDS[lastPos].reportData.last().ATPTimestamp);

		res[0][0] = time_start.toString("yy-mm-dd");
		emit WriteData(0, QRect(2, 2, 1, 1), res, Qt::white);  //试验日期

		QString time_start_s = time_start.toString("hh:mm:ss");
		QString time_end_s = time_end.toString("hh:mm:ss");
		res[0][0] = QString("%1-%1").arg(time_start_s).arg(time_end_s);
		emit WriteData(0, QRect(2, 3, 1, 1), res, Qt::white);  //起止时间
	}

	res[0][0] = QString::number(V_SDS.size());
	emit WriteData(0, QRect(2, 4, 1, 1), res, Qt::white);  //试验序列数

															//TODO 检测里程
	//开始统计并填写各个试验序列的数据
	QVector<int> row_positionVec;
	row_positionVec.resize(7);
	row_positionVec[0] = 9;
	row_positionVec[1] = 1;
	row_positionVec[2] = 1;
	row_positionVec[3] = 1;
	row_positionVec[4] = 1;
	row_positionVec[5] = 1;
	row_positionVec[6] = 1;


	QVector<CompareType> ctVec;
	ctVec << C_YINGDAQIWEIZHI << C_GUIDAODIANLU << C_XIANLUSUDU << C_XIANLUPODU << C_CHEZHAN << C_FENXIANGQU;

	for (int i = 0; i < V_SDS.size(); i++)
	{

		//向主表对应位置写入一次测试序列信息
		res[0][0] = QString::number(i + 1);  //序号
		emit WriteData(0, QRect(1, row_positionVec[0], 1, 1), res, Qt::white);

		res[0][0] = V_SDS[i].sequenceNumber; //试验序列号
		emit WriteData(0, QRect(2, row_positionVec[0], 1, 1), res, Qt::white);

		if (V_SDS[i].reportData.isEmpty())
		{
			emit ProgressStep(QString("%1 数据统计完成").arg(V_SDS[i].sequenceNumber), 100 / V_SDS.size());
			row_positionVec[0]++;
			continue;
		}

		QDateTime start = QDateTime::fromTime_t(V_SDS[i].reportData.front().ATPTimestamp);

		QDateTime end = QDateTime::fromTime_t(V_SDS[i].reportData.last().ATPTimestamp);

		QString start_s = start.toString("hh:mm:ss");
		QString end_s = end.toString("hh:mm:ss");
		res[0][0] = QString("%1-%2").arg(start_s).arg(end_s);
		emit WriteData(0, QRect(3, row_positionVec[0], 1, 1), res, Qt::white);  //起止时间



		for (int j = 0; j < 6; j++)
		{
			QVector<int> errorIndex = ErrorFilter(V_SDS[i], ctVec[j]);

			//先写测试序列号
			emit CombineCell(j + 1, QRect(1, row_positionVec[j + 1], 6, 1), xlCenter);
			res[0][0] = V_SDS[i].sequenceNumber;
			emit WriteData(j + 1, QRect(1, row_positionVec[j + 1], 1, 1), res, Qt::green);

			//向主表对应位置添加统计信息和超链接
			res[0][0] = QString::number(errorIndex.size());
			emit WriteData(0, QRect(4 + j, row_positionVec[0], 1, 1), res, Qt::white);
			//TODO超链接

			emit AddHyperlink(0, QRect(4 + j, row_positionVec[0], 1, 1), j + 1, QRect(1, row_positionVec[j + 1], 1, 1));

			row_positionVec[j + 1]++;  //写完一行行位置加1
			row_positionVec[j + 1]++;  //空一行

			//分别写应答器报文和比对结果
			QVector<QVector<QVector<QString> > > baliseDataVec;
			QVector<QVector<QVector<QString> > > compareResultVec;
			QVector<QString> baliseIDVec;
			baliseDataVec = GetErrorBaliseData(V_SDS[i], errorIndex, ctVec[j], baliseIDVec);
			compareResultVec = GetErrorCompareResult(V_SDS[i], errorIndex, ctVec[j]);

			for (int p = 0; p < baliseDataVec.size(); p++)
			{
				res[0][0] = QString("%1 应答器报文").arg(baliseIDVec[p]);
				emit CombineCell(j + 1, QRect(1, row_positionVec[j + 1], 6, 1), xlLeft);
				emit WriteData(j + 1, QRect(1, row_positionVec[j + 1], 1, 1), res, Qt::green);
				row_positionVec[j + 1]++;  //写完一行行位置加1
				emit WriteData(j + 1, QRect(1, row_positionVec[j + 1], baliseDataVec[p][0].size(), baliseDataVec[p].size()), baliseDataVec[p], Qt::white); //写应答器报文
				row_positionVec[j + 1] += baliseDataVec[p].size();
				row_positionVec[j + 1]++; //空一行

				res[0][0] = QString("%1 比对结果").arg(baliseIDVec[p]);
				emit CombineCell(j + 1, QRect(1, row_positionVec[j + 1], 6, 1), xlLeft);
				emit WriteData(j + 1, QRect(1, row_positionVec[j + 1], 1, 1), res, Qt::green);
				row_positionVec[j + 1]++;  //写完一行行位置加1
				emit WriteData(j + 1, QRect(1, row_positionVec[j + 1], compareResultVec[p][0].size(), compareResultVec[p].size()), compareResultVec[p], Qt::white); //写应答器报文
				row_positionVec[j + 1] += compareResultVec[p].size();
				row_positionVec[j + 1]++; //空一行
			}
			emit ProgressStep(QString("%1 - %2统计完成").arg(V_SDS[i].sequenceNumber).arg(nameList[j + 1]), 100 / V_SDS.size() / 6);
		}
		row_positionVec[0]++;
	}
	emit SaveAs(generateFilePath);
	emit ProgressStep(QString("统计完成"), 100);
}

void GetReport_DIALOG::on_pushButton_GetSelectedFile_clicked()
{
    QList<QTreeWidgetItem*> selectedItemList = ui->treeWidget_FIleList->selectedItems();
	QVector<QString> v_str;
	QString arg;
	if (ui->comboBox->currentText() == "数据文件")
		arg = "data";
	else if (ui->comboBox->currentText() == "报表文件")
		arg = "report";
	else
		return;
	for (auto& selectedItem : selectedItemList)
		if (selectedItem->parent() != nullptr)
			v_str.push_back(selectedItem->parent()->text(0) + "/" + selectedItem->text(0));
	if (v_str.isEmpty())
		return;
	if (arg == "data" && v_str.size() != 1)//前提条件判定
	{
		QMessageBox::information(this, "提示", "数据文件只能并且必须选择其中一个");
		return;
	}
	if (arg == "report")
	{
		generateFilePath = QFileDialog::getSaveFileName(this, "请选择报表生成路径", QCoreApplication::applicationDirPath(), "ExcelFile (*.xlsx)");
		if (generateFilePath.isEmpty())
		{
			return;
		}
	}
    tcpCommandClient->send(Combine_Command_Data(TcpHead(CMD_FROM::CLIENT, CMD_TYPE::CONTROL, CMD_NAME::GET_SELECTED_FILE), arg, v_str));
}

void GetReport_DIALOG::on_treeWidget_FIleList_clicked(const QModelIndex &index)
{
    qDebug()<<index.row();

}

void GetReport_DIALOG::on_treeWidget_FIleList_itemClicked(QTreeWidgetItem *item, int column)//点击一级目录时选择所有子项
{
	qDebug() << item->text(0);
    if(item->parent()!=nullptr||column!=0)
        return;
    int count_selected=0;
    for(int i=0;i<item->childCount();++i)
    {
        item->child(i)->isSelected()==true?++count_selected:0;
    }
    if(count_selected==item->childCount())
    {
        for(int i=0;i<item->childCount();++i)
        {
            item->child(i)->setSelected(false);
        }
    }
    else
    {
        for(int i=0;i<item->childCount();++i)
        {
            if(item->child(i)->isSelected()==false)
                item->child(i)->setSelected(true);
        }
    }
    item->setSelected(false);
}

void GetReport_DIALOG::AddProgressAndLog(QString log, int progressValue)
{
	ui->label_GenerateLog->setText(log);
	ui->progressBar->setValue(progressValue);
	if (progressValue == 100) {
		updateStatus("green", "已完成");
	}
}


void GetReport_DIALOG::errorHandler(int errorCode)
{
	switch (errorCode)
	{
	case 0:
		QMessageBox::information(this, "提示", "打开Excel失败!");
		break;
	default:
		break;

	}
}

QVector<int> GetReport_DIALOG::ErrorFilter(const SequenceDataStructure &sds, CompareType ct)
{
    QVector<int> errorRecord;
    for(int n = 0; n < sds.reportData.size(); n++)
    {
        QVector<QVector<QString> > compareResult = sds.reportData[n].Compare_Result[ct];
        bool hasError = false;
        if (ct == C_YINGDAQIWEIZHI || ct == C_CHEZHAN )
        {
			if (!compareResult.isEmpty())
			{
				if (compareResult[0][2] == "Wrong")
					hasError = true;
			} 
        }
        else
        {
            for (int i = 0; i < compareResult.size(); i++)
            {
                for (int j = 0; j < compareResult[i].size(); j++)
                {
                    if (compareResult[i][j].contains("不一致") || compareResult[i][j] == "无匹配")
                    {
                        hasError = true;
                        break;
                    }
                }
                if (hasError)
                {
                    break;
                }
            }
        }
        if(hasError)
        {
            errorRecord.push_back(n);
        }
    }
	return errorRecord;
}

QVector<QVector<QVector<QString>> > GetReport_DIALOG::GetErrorBaliseData(SequenceDataStructure &sds, QVector<int> errorRecord, CompareType ct, QVector<QString> &baliseIDVec)
{
	QVector<QVector<QVector<QString>> > forReturn;
	for (int i = 0; i < errorRecord.size(); i++)
	{
		//解析应答器报文
		QVector<QVector<Item> > packVec = Analyze_BaliseMessage::Resolve(sds.reportData[errorRecord[i]].BalishData);
		QString baliseID = Analyze_BaliseMessage::GetBaliseID(sds.reportData[errorRecord[i]].BalishData);
		QString packageName = "";
		switch (ct) {
		case C_YINGDAQIWEIZHI:
			packageName = "ETCS-5";
			break;
		case C_GUIDAODIANLU:
			packageName = "ETCS-44";
			break;
		case C_XIANLUSUDU:
			packageName = "ETCS-27";
			break;
		case C_XIANLUPODU:
			packageName = "ETCS-21";
			break;
		case C_CHEZHAN:
			packageName = "ETCS-72";
			break;
		case C_FENXIANGQU:
			packageName = "ETCS-68";
			break;
		default:
			break;
		}
		QVector<int> positionVec = Analyze_BaliseMessage::GetPackagePosition(packageName, packVec);
		QVector<Item> package;

		for (int k = 0; k < positionVec.size(); k++)
		{
			QVector<Item> Q_DIR_Vec = Analyze_BaliseMessage::FindItem("Q_DIR", packVec[positionVec[k]]);
			if (Q_DIR_Vec[1].value == sds.reportData[errorRecord[i]].Direction) //只筛选出用来比对的应答器包
			{
				package = packVec[positionVec[k]];
			}
		}


		//将QVector<Item>数据转化为QVector<QVector<QString> > 
		QVector<QVector<QString> > baliseResult;
		if (positionVec.size() == 0 && ct == C_YINGDAQIWEIZHI)  //出现应答器位置比对错误时，可能没有E5包
		{
			baliseResult.resize(1);
			baliseResult[0].resize(1);
			baliseResult[0][0] = "无E5包";
		}
		else {
			baliseResult.resize(package.size() - 1);
			for (int t = 0; t < baliseResult.size(); ++t)
			{
				baliseResult[t].resize(3);
			}
			for (int p = 0; p < baliseResult.size(); p++)
			{
				baliseResult[p][0] = package[p + 1].remark;
				baliseResult[p][1] = package[p + 1].name;
				baliseResult[p][2] = QString::number(package[p + 1].value);
			}
		}
		forReturn.push_back(baliseResult);
		baliseIDVec.push_back(baliseID);
	}

	return forReturn;
}

QVector<QVector<QVector<QString> > > GetReport_DIALOG::GetErrorCompareResult(const SequenceDataStructure &sds, QVector<int> errorRecord, CompareType ct)
{
    QVector<QVector<QVector<QString>> > forReturn;
    for(int i = 0; i < errorRecord.size(); i++)
    {
        QVector<QVector<QString> > temp_Vec;
        QVector<QVector<QString> > compareResult;
        temp_Vec = sds.reportData[errorRecord[i]].Compare_Result[ct];
        //这里需要对表格内容做统一格式处理，使其写入excel
        /*if(ct == C_YINGDAQIWEIZHI || ct == C_CHEZHAN)
        {

        }*/
        /*else
        {*/
            //丢掉第一行
            for(int i = 1; i < temp_Vec.size(); i++)
            {
                compareResult.push_back(temp_Vec[i]);
            }
        /*}*/
        forReturn.push_back(compareResult);
    }
    return forReturn;

}

void GetReport_DIALOG::updateStatus(QString color, QString content)//更新底部状态栏信息
{
    msgLabel->setStyleSheet(" QLabel{ color: "+color+" }");
    msgLabel->setText(content);
}

void GetReport_DIALOG::on_comboBox_currentIndexChanged(const QString &arg1)
{
    ui->treeWidget_FIleList->clear();
    QString suffix;
    if(arg1 == "全部文件")
        suffix = "";
    else if(arg1 == "数据文件")
        suffix = "dmsCompareData";
    else if(arg1 == "报表文件")
        suffix = "dmsSummary";
    else if(arg1 == "原始文件")
        suffix = "dmsBinData";
    else
        return;
    QList<QTreeWidgetItem*> temp_treeitem;
    for(int j=0;j<FileList.size();++j){
        ui->treeWidget_FIleList->addTopLevelItem(new QTreeWidgetItem(QStringList(FileList[j][0])));
        temp_treeitem.clear();
        for(int i=1;i<FileList[j].size();++i)
        {
            if(FileList[j][i].endsWith(suffix))
                temp_treeitem.push_back(new QTreeWidgetItem(QStringList(FileList[j][i])));
        }
        ui->treeWidget_FIleList->topLevelItem(j)->addChildren(temp_treeitem);
    }
}
