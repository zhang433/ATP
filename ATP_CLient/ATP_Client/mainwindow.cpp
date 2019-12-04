#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include <QMetaType>
#include <QDebug>
#include <algorithm>
#include <QMessageBox>
#include <vector>
#include <iostream>
#include <QInputDialog>
#include <fstream>
#include <math.h>
#include <time.h>
#include <QFileDialog>
#include <QPainter>
#include <QThread>
#include <QMouseEvent>
#include <QPushButton>
#include <QtAlgorithms>
#include "showresponsermsg_dialog.h"
#include "self_define.h"
#include "analyze_resmessage.h"
#include <QResource>
#include <analyze_rbcmessage.h>
#include "designdata.h"
#include <QDir>
#include <QStringList>
#include "getreport_dialog.h"
#include "tcpcommandclient.h"
#include "usermanagementui.h"

//QString ARM_IP = "127.0.0.1";
extern QString EXE_VERSION;
extern QString REMOTE_VERSION;

QString ARM_IP = "192.168.2.1";

const QString MainWindow::DMS_DONGZUOMOSHI_LIST[20] = { "FS[完全监控]","PS[部分监控]","RO[反向运行]","CO[引导]","CS[机车信号]","BF[应答器故障]","OS[目视]","SR[人控]","SH[调车]","UN[未装备]","SL[休眠]","SB[待机]","TR[冒进]","PT[冒进后]","SF[系统故障]","IS[隔离]","NL[非本务]","SE[欧洲 STM]","SN[国家 STM]","RV[退行]" };
const QString MainWindow::DMS_ZAIPIN_LIST[10] = { "Unknow","0","550","650","750","850","1700","2000","2300","2600" };
const QString MainWindow::_200C_ZAIPIN_LIST[19] = { "NO_CODE","L3","L","L2","LU","U2","LU2","U","UU","UUS","U2S","L5","U3","L4","HB","25.7","HU","27.9","H" };
const QString MainWindow::DMS_DIPIN_LIST[21] = { "Unknow","0","H","HU","HB","U","U2S","U2","UU","UUS","LU","LU2","L","L2","L3","L4","L5","L6","25.7","27.9","U3" };
const QString MainWindow::_200C_DIPIN_LIST[34] = { "F_0000","F_1700","F_2000","F_2300","F_2600","F1_1700","F2_1700","F1_2000","F2_2000","F1_2300","F2_2300","F1_2600","F2_2600","F_550","F_650","F_750","F_850","F_550_750","F_650_850","F_1698p7_1701p4","F_1998p7_2001p4","F_2298p7_2301p4","F_2598p7_2601p4","F_1700_2300","F_2000_2600","F_1698p7_2298p7","F_1698p7_2301p4","F_1701p4_2298p7","F_1701p4_2301p4","F_1998p7_2598p7","F_1998p7_2601p4","F_2001p4_2598p7","F_2001p4_2601p4","F_Unknown" };
const char MainWindow::mask_novalue[8] = { static_cast<char>(0xFF),static_cast<char>(0xFF),static_cast<char>(0xFF),static_cast<char>(0xFF),static_cast<char>(0xFF),static_cast<char>(0xFF),static_cast<char>(0xFF),static_cast<char>(0xFF) };

QQueue<RealTimeDatastructure> MainWindow::RTD_Queue;
QMutex MainWindow::m_static_mutex;
QMutex MainWindow::wait_Mutex;
QWaitCondition MainWindow::wait_Condition;

Train_Dir g_TrainDirection;
Line_Dir g_LineDirection;

int COUNT = 0;

MainWindow::MainWindow(QString user,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    UserName(user)
{

    ui->setupUi(this);
    this->setCentralWidget(ui->tabWidget);

	connect(ui->Draw_FRAME, &ChartView::AnalysisInfoClick_SIGNAL, this, &MainWindow::AnalysisInfoClick_SLOT);
    connect(tcpCommandClient,&TcpCommandClient::version_Error,this,&MainWindow::show_VersionError);
    //connect(tcpCommandClient, SIGNAL(cleanView_SIGNAL()), this, SLOT(on_ClearLocal_triggered()));
    ui->ATPSHIJIANDATE->setSegmentStyle(QLCDNumber::Flat);
    ui->ATPSHIJIANTIME->setSegmentStyle(QLCDNumber::Flat);

    //隐藏里程与公里标选项卡以及UDP调试信息选项卡
    ConnectStateLabel_Command = new QLabel(this);
    ConnectStateLabel_Command->setStyleSheet(" QLabel{ color: red }");
    ConnectStateLabel_Command->setText("命令:未连接");
    ui->statusBar->addWidget(ConnectStateLabel_Command);

	ConnectStateLabel_Data = new QLabel(this);
	ConnectStateLabel_Data->setStyleSheet(" QLabel{ color: red }");
	ConnectStateLabel_Data->setText("数据:未连接");
	ui->statusBar->addWidget(ConnectStateLabel_Data);

    SequenceLabel = new QLabel(this);
	SequenceLabel->setStyleSheet(" QLabel{ color: black }");
    SequenceLabel->setText("车次:UnknownSequence");
    ui->statusBar->addWidget(SequenceLabel);

	BetteryLabel = new QLabel(this);
	BetteryLabel->setStyleSheet(" QLabel{ color: black }");
	BetteryLabel->setText("电量:??%");
	ui->statusBar->addWidget(BetteryLabel);

	SDCardLabel = new QLabel(this);
	SDCardLabel->setStyleSheet(" QLabel{ color: black }");
	SDCardLabel->setText("SD卡:??%");
	ui->statusBar->addWidget(SDCardLabel);

	qRegisterMetaType<STATUS_BAR>("STATUS_BAR");
    qRegisterMetaType< QVector<QVector<QString> > >("QVector<QVector<QString> >");
	qRegisterMetaType<QVector<QString>>("QVector<QString> >");
	qRegisterMetaType <QVector<QByteArray>> ("QVector<QByteArray> >");
    qRegisterMetaType<CompareType>("CompareType");
    qRegisterMetaType<QVector<SequenceDataStructure>>("QVector<SequenceDataStructure>");

    GR_DIALOG = new GetReport_DIALOG(this);

    ui->tabWidget->setCurrentIndex(0);
}

/* 用于初始化绘图
 *
 *
 * *****************************************/


QTreeWidgetItem * AddTreeNode(QTreeWidgetItem *parent, QString name, QString desc)
{
    QTreeWidgetItem * item = new QTreeWidgetItem(QStringList() << name << desc);
    parent->addChild(item);
    return item;
}

MainWindow::~MainWindow()
{
    delete ui;
    exit(0);
}

void MainWindow::UpdateStatusState_SLOT(QString msg, QString backgroud_color, enum STATUS_BAR status_bar)
{
	QLabel* tempLable;
	switch (status_bar)
	{
	case STATUS_BAR::COMMAND_SATUS:
		tempLable = ConnectStateLabel_Command;
		break;
	case STATUS_BAR::DATA_STATUS:
		tempLable = ConnectStateLabel_Data;
		break;
	case STATUS_BAR::TRAIN_SEQUENCE:
		tempLable = SequenceLabel;
		break;
	case STATUS_BAR::BETTERY_STATUS:
		tempLable = BetteryLabel;
		break;
	case STATUS_BAR::SD_CAPACITY:
		tempLable = SDCardLabel;
		break;
	default:
		assert(false);
		return;
	}
	tempLable->setStyleSheet(backgroud_color);
	tempLable->setText(msg);
    if(msg=="命令:已连接")
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setText("校验版本");
        msgBox.setInformativeText("正在核对板卡程序版本，等待远程回应，请勿操作");
        msgBox.setStandardButtons(QMessageBox::NoButton);
        msgBox.show();
        QApplication::processEvents();
        wait_Mutex.lock();
        VersionIsVerfied = false;
        tcpCommandClient->send(Combine_Command_Data(TcpHead(CMD_FROM::CLIENT, CMD_TYPE::CONTROL, CMD_NAME::VERSION)));
        bool ret = wait_Condition.wait(&wait_Mutex,5000);
        msgBox.close();
        if(!ret)
        {
            QMessageBox::critical(this,"错误","远端无反应，版本校验失败，程序将强制退出！");
            exit(0);
        }
        else
        {
            auto sp_remote = REMOTE_VERSION.split(".");
            auto sp_this = EXE_VERSION.split(".");
            if(sp_remote.size()!=sp_this.size() || sp_remote[0]!=sp_this[0] || sp_remote[1]!=sp_this[1])
            {
                QMessageBox::critical(nullptr,"错误","版本错误，远端版本号为："+REMOTE_VERSION);
                exit(0);
            }
            QMessageBox::information(this,"提示","版本校验成功！远端程序版本号为："+REMOTE_VERSION);
        }
        wait_Mutex.unlock();
        VersionIsVerfied = true;
    }
}

/***********************************************

** 主界面内容填充

*************************************************/
void MainWindow::ReDraw_MainWindow()
{
    if(!VersionIsVerfied)
        return;
	static RealTimeDatastructure static_RTD;

	if (MainWindow::RTD_Queue.empty())
		return;
	QVector<RealTimeDatastructure> V_RTD;
	{
		QMutexLocker locker(&MainWindow::m_static_mutex);
		while (!MainWindow::RTD_Queue.empty())
		{
			V_RTD.push_back(MainWindow::RTD_Queue.front());
			if (RTD_Queue.front().Has_Balish_Compare_Result)
				UpdateBaliseCompareResult(RTD_Queue.front());
			MainWindow::RTD_Queue.pop_front();
		}
	}

	auto UpdateRTD = [this](RealTimeDatastructure& old_RTD,const RealTimeDatastructure& new_RTD)->void {
		old_RTD.Has_200C_DATA = false;
		old_RTD.Has_Balish_Compare_Result = false;
		old_RTD.Has_DMS_ATP_DATA = false;
		old_RTD.Has_DMS_RBC_DATA = false;
		//必有的数据
		if (!MEANINGLESS(new_RTD.ATPTimestamp))
			old_RTD.ATPTimestamp = new_RTD.ATPTimestamp;
		if (!MEANINGLESS(new_RTD.IPCTimestamp))
			old_RTD.IPCTimestamp = new_RTD.IPCTimestamp;
		//ATP基础数据，DMS数据和200C数据的共同部分
		old_RTD.Has_DMS_ATP_DATA = new_RTD.Has_DMS_ATP_DATA;
		if (new_RTD.Has_DMS_ATP_DATA)
		{
			if (!MEANINGLESS(new_RTD.Car_ID))
				old_RTD.Car_ID = new_RTD.Car_ID;
			if (!MEANINGLESS(new_RTD.CTCS_LEVEL_EDIT))
				old_RTD.CTCS_LEVEL_EDIT = new_RTD.CTCS_LEVEL_EDIT;
			if (!MEANINGLESS(new_RTD.CTCS_MODE_EDIT))
				old_RTD.CTCS_MODE_EDIT = new_RTD.CTCS_MODE_EDIT;
			if (!MEANINGLESS(new_RTD.BREAK_COMMAND))
				old_RTD.BREAK_COMMAND = new_RTD.BREAK_COMMAND;
			if (!MEANINGLESS(new_RTD.MAINT_VIT_TRAIN_LCD))
				old_RTD.MAINT_VIT_TRAIN_LCD = new_RTD.MAINT_VIT_TRAIN_LCD;
			if (!MEANINGLESS(new_RTD.MAINT_KV_VIT_CTRL_FU))
				old_RTD.MAINT_KV_VIT_CTRL_FU = new_RTD.MAINT_KV_VIT_CTRL_FU;
			if (!MEANINGLESS(new_RTD.MAINT_VIT_PRE_INTERV_LCD))
				old_RTD.MAINT_VIT_PRE_INTERV_LCD = new_RTD.MAINT_VIT_PRE_INTERV_LCD;
			if (!MEANINGLESS(new_RTD.MAINT_VIT_OBJECTIF_LCD))
				old_RTD.MAINT_VIT_OBJECTIF_LCD = new_RTD.MAINT_VIT_OBJECTIF_LCD;
			if (!MEANINGLESS(new_RTD.MAINT_DIS_OBJECTIF))
				old_RTD.MAINT_DIS_OBJECTIF = new_RTD.MAINT_DIS_OBJECTIF;
			if (!MEANINGLESS(new_RTD.MAINT_POS_GEO_EDIT))
				old_RTD.MAINT_POS_GEO_EDIT = new_RTD.MAINT_POS_GEO_EDIT;
			if (!MEANINGLESS(new_RTD.CTCS_CODE_EDIT))
				old_RTD.CTCS_CODE_EDIT = new_RTD.CTCS_CODE_EDIT;
			if (!MEANINGLESS(new_RTD.CTCS_CARRIER_EDIT))
				old_RTD.CTCS_CARRIER_EDIT = new_RTD.CTCS_CARRIER_EDIT;
			if (!MEANINGLESS(new_RTD.CTCS_DRIVER_ID))
				old_RTD.CTCS_DRIVER_ID = new_RTD.CTCS_DRIVER_ID;
		}
		old_RTD.Has_200C_DATA = new_RTD.Has_200C_DATA;
		if (new_RTD.Has_200C_DATA)
		{
			//TO DO
		}
		old_RTD.Has_DMS_RBC_DATA = new_RTD.Has_DMS_RBC_DATA;
		if (new_RTD.Has_DMS_RBC_DATA)
		{
			old_RTD.RBC = new_RTD.RBC;
		}
		old_RTD.Has_Balish_Compare_Result = new_RTD.Has_Balish_Compare_Result;
		if (new_RTD.Has_Balish_Compare_Result)
		{
			old_RTD.BalishData = new_RTD.BalishData;
			old_RTD.Compare_Result = new_RTD.Compare_Result;
		}
	};

	for (auto & input : V_RTD)
	{
        ui->Draw_FRAME->UpdateView(input,move_frame);//更新视图
        /**********************************************/
        static QSet<QVector<qint8>> m;
        if(input.Has_Balish_Compare_Result && Analyze_BaliseMessage::CheckBaliseInfoIllegal(input.BalishData))
        {
            QVector<qint8> v;
            for(int i=0;i<104;++i)
                v.push_back(input.BalishData.Balise_Tel[i]);
            m.insert(v);
            for(auto& v:m)
            {
                QString BinaryMessage;
                for(unsigned int i=0;i<v.size();++i)
                {
                    BinaryMessage.append(QString("%1").arg((unsigned char)v[i],2,16,QChar('0')).toUpper()+" ");
                }
                qDebug()<<BinaryMessage;
            }
            qDebug()<<"**************************************";
        }
        /***********************************************/
	}
    if(!move_frame)
        ui->Draw_FRAME->moveCenturalToPoint(V_RTD.back().IPCTimestamp);
    move_frame = true;
	UpdateRTD(static_RTD, V_RTD.back());
	UpdateView(static_RTD);
}

void MainWindow::UpdateView(RealTimeDatastructure &RTD)
{
    static QDateTime QDT;
    QDT.setMSecsSinceEpoch(RTD.ATPTimestamp);
    if(RTD.Has_DMS_ATP_DATA)//共有的数据部分
    {
        QString temp_str;
        bool from_200C = false;
        if(RTD.Has_200C_DATA)//如果同时有DMS_ATP和200C数据，那么这条数据就是200C提供的，按照200C协议解析
            from_200C=true;
        ui->ATPSHIJIANDATE->display(QDT.toString("yyyy-MM-dd"));//当前atp时间
        ui->ATPSHIJIANTIME->display(QDT.toString("hh:mm:ss"));//当前atp时间
        double temp_speed = RTD.MAINT_VIT_TRAIN_LCD;
        if(from_200C)
            temp_speed*=0.036;//200C中的数据单位是cm/s
        temp_str = QString::number(temp_speed);
        if(!from_200C&&MEANINGLESS(RTD.MAINT_VIT_TRAIN_LCD))
            temp_str="无法提供";
        ui->DANGQIANSUDU->display(temp_str);//当前速度
        temp_speed = RTD.MAINT_KV_VIT_CTRL_FU;
        if(from_200C)
            temp_speed*=0.036;//200C中的数据单位是cm/s
        temp_str = QString::number(temp_speed);
        if(!from_200C&&MEANINGLESS(RTD.MAINT_KV_VIT_CTRL_FU))
            temp_str="无法提供";
        ui->EBPSUDU->display(temp_str);//EBP速度
        temp_speed = RTD.MAINT_VIT_PRE_INTERV_LCD;
        if(from_200C)
            temp_speed*=0.036;//200C中的数据单位是cm/s
        temp_str = QString::number(temp_speed);
        if(!from_200C&&MEANINGLESS(RTD.MAINT_VIT_PRE_INTERV_LCD))
            temp_str="无法提供";
        ui->NBPSUDU->display(temp_str);//NBP速度
        temp_speed = RTD.MAINT_VIT_OBJECTIF_LCD;
        if(from_200C)
            temp_speed*=0.036;//200C中的数据单位是cm/s
        temp_str = QString::number(temp_speed);
        if(!from_200C&&MEANINGLESS(RTD.MAINT_VIT_OBJECTIF_LCD))
            temp_str="无法提供";
        ui->MUBIAOSUDU->display(temp_str);//目标速度

        temp_str.resize(8);
		temp_str.clear();
        if(from_200C)
        {
            temp_str[0]=QChar(char(RTD.Car_ID>>56));
            temp_str[1]=QChar(char(RTD.Car_ID>>48));
            temp_str[2]=QChar(char(RTD.Car_ID>>40));
            temp_str[3]=QChar(char(RTD.Car_ID>>32));
            temp_str[4]=QChar(char(RTD.Car_ID>>24));
            temp_str[5]=QChar(char(RTD.Car_ID>>16));
            temp_str[6]=QChar(char(RTD.Car_ID>>8));
            temp_str[7]=QChar(char(RTD.Car_ID>>0));
        }
        else
        {
			for (int i = 7; i >= 0; --i)
			{
				if (char(RTD.Car_ID >> (8 * i)))
				{
					if (i >= 4)
						temp_str.push_back(QChar(char(RTD.Car_ID >> (8 * i))));
					else 
						temp_str.push_back(QString::number(quint8(RTD.Car_ID >> (8 * i))));
				}
			}
            if(memcmp(&RTD.Car_ID,mask_novalue,sizeof(RTD.Car_ID))==0)
                temp_str="无法提供";
        }
        ui->CHECI->setText(temp_str);//车次号
        switch(RTD.CTCS_LEVEL_EDIT)
        {
        case 0:
            temp_str = "CTCS-0";
            break;
        case 1:
            temp_str = "CTCS-1";
            break;
        case 2:
            temp_str = "CTCS-2";
            break;
        case 3:
            temp_str = "CTCS-3";
            break;
        case 4:
            temp_str = "CTCS-4";
            break;
        default:
            if(!from_200C&&memcmp(&RTD.CTCS_LEVEL_EDIT,mask_novalue,sizeof(RTD.CTCS_LEVEL_EDIT))==0)
                temp_str="无法提供";
            else
                temp_str = "预期外的值";
            break;
        }
        ui->DENGJI->setText(temp_str);//列控等级
        if(from_200C)
        {
            switch (RTD.CTCS_MODE_EDIT)//当前控车模式
            {
            case 0:
                temp_str = "LKJ";
                break;
            case 6:
                temp_str = "SB";
                break;
            case 5:
                temp_str = "SH";
                break;
            case 9:
                temp_str = "CO";
                break;
            case 1:
                temp_str = "FS";
                break;
            case 4:
                temp_str = "OS";
                break;
            case 11:
                temp_str = "TR";
                break;
            case 12:
                temp_str = "PT";
                break;
            case 255:
                temp_str = "NP";
                break;
            default:
                temp_str = "预期外的值";
                break;
            }
        }
        else
        {
            if(memcmp(&RTD.CTCS_MODE_EDIT,mask_novalue,sizeof(RTD.CTCS_MODE_EDIT))==0)
                temp_str="无法提供";
            else
            {
                if(RTD.CTCS_MODE_EDIT>sizeof(DMS_DONGZUOMOSHI_LIST)/sizeof(DMS_DONGZUOMOSHI_LIST[0]))
                    temp_str = "预期外的值";
                else
                    temp_str = DMS_DONGZUOMOSHI_LIST[RTD.CTCS_MODE_EDIT];
            }
        }
        ui->DONGZUOMOSHI->setText(temp_str);//控车模式
        switch(RTD.BREAK_COMMAND)
        {
        case 0:
            temp_str = "无";
            break;
        case 1:
            temp_str = "SB1[一级常用制动]";
            break;
        case 4:
            temp_str = "SB4[四级常用制动]";
            break;
        case 7:
            temp_str = "SB7[七级常用制动]";
            break;
        case 13:
            temp_str = "EB[紧急制动]";
            break;
        default:
            if(!from_200C&&memcmp(&RTD.BREAK_COMMAND,mask_novalue,sizeof(RTD.BREAK_COMMAND))==0)
                temp_str="无法提供";
            else
            {
                temp_str = "预期外的值";
                qDebug()<<"制动信息"<<RTD.BREAK_COMMAND;
            }
            break;
        }
        ui->ZHIDONGZHILING->setText(temp_str);//制动信息
        temp_str = QString::number(RTD.MAINT_DIS_OBJECTIF)+"m";
        if(!from_200C&&memcmp(&RTD.MAINT_DIS_OBJECTIF,mask_novalue,sizeof(RTD.MAINT_DIS_OBJECTIF))==0)
            temp_str="无法提供";
        ui->MUBIAOJULI->setText(temp_str);//目标距离
        temp_str = QString::number(RTD.MAINT_POS_GEO_EDIT)+"m";
        if(!from_200C&&memcmp(&RTD.MAINT_POS_GEO_EDIT,mask_novalue,sizeof(RTD.MAINT_POS_GEO_EDIT))==0)
            temp_str="无法提供";
        ui->LICHENG->setText(temp_str);
        if(from_200C)
        {
            if(RTD.CTCS_CODE_EDIT>=sizeof(_200C_ZAIPIN_LIST)/sizeof(_200C_ZAIPIN_LIST[0]))
                temp_str="预期外的值";
            else
                temp_str = _200C_ZAIPIN_LIST[RTD.CTCS_CODE_EDIT];
        }
        else
        {
            if(memcmp(&RTD.CTCS_CODE_EDIT,mask_novalue,sizeof(RTD.CTCS_CODE_EDIT))==0)
                temp_str="无法提供";
            else
            {
                if(RTD.CTCS_CODE_EDIT>=sizeof(DMS_ZAIPIN_LIST)/sizeof(DMS_ZAIPIN_LIST[0]))
                    temp_str="预期外的值";
                else
                    temp_str = DMS_ZAIPIN_LIST[RTD.CTCS_CODE_EDIT];
            }
        }
        ui->ZAIPIN->setText(temp_str);
        if(from_200C)
        {
            if(RTD.CTCS_CARRIER_EDIT>=sizeof(_200C_DIPIN_LIST)/sizeof(_200C_DIPIN_LIST[0]))
                temp_str="预期外的值";
            else
                temp_str = _200C_DIPIN_LIST[RTD.CTCS_CARRIER_EDIT];
        }
        else
        {
            if(memcmp(&RTD.CTCS_CARRIER_EDIT,mask_novalue,sizeof(RTD.CTCS_CARRIER_EDIT))==0)
                temp_str="无法提供";
            else
            {
                if(RTD.CTCS_CARRIER_EDIT>=sizeof(DMS_DIPIN_LIST)/sizeof(DMS_DIPIN_LIST[0]))
                    temp_str="预期外的值";
                else
                    temp_str = DMS_DIPIN_LIST[RTD.CTCS_CARRIER_EDIT];
            }
        }
        ui->DIPIN->setText(temp_str);//低频
        temp_str = QString::number(RTD.CTCS_DRIVER_ID);
        if(!from_200C&&memcmp(&RTD.CTCS_DRIVER_ID,mask_novalue,sizeof(RTD.CTCS_DRIVER_ID))==0)
            temp_str="无法提供";
        ui->SIJIID->setText(temp_str);
    }
    else if(RTD.Has_DMS_RBC_DATA)
    {
//        qDebug()<<"execute Resolve in main";
//        PackCollectorRBC PCR = Analyze_RBCMessage::Resolve(RTD.RBC);
//        if(PCR[0].size()==1)
//        {
//            qDebug()<<"Analyze RBC Data failed.";
//        }
        //Change_AnalysisResult(nullptr,&(RTD.RBC));
    }
    if(RTD.Has_200C_DATA)
    {
        QString temp_str;
        temp_str = QString::number(RTD.m_MAIN_POS_TRAIN_LCD*0.036);
        ui->ZIZOUXINGJULI->setText(temp_str);
        temp_str = QString::number(RTD.MAINT_VIT_PERMISE_LCD*0.036)+"km/h";
        ui->YUNXUSUDU->setText(temp_str);
        switch(RTD.CTCS_TRAIN_LENGTH_EDIT)
        {
        case 0:
            temp_str = "未知";
            break;
        case 1:
            temp_str = "单组";
            break;
        case 2:
            temp_str = "双组";
            break;
        default:
            temp_str = "预期外的值";
            break;

        }
        ui->CHECHANG->setText(temp_str);
        temp_str = QString::number(RTD.MAINT_L_DOUBTUNDER_EDIT*0.01)+"m";
        ui->ZUIDACHETOUWUCHA->setText(temp_str);
        temp_str = QString::number(RTD.MAINT_L_DOUBTOVER_EDIT*0.01)+"m";
        ui->ZUIXIAOCHETOUWUCHA->setText(temp_str);
        temp_str = QString::number(RTD.CTCS_D_EOA_EDIT*0.01)+"m";
        ui->MACHANGDU->setText(temp_str);
        temp_str = QString::number(RTD.CTCS_L_TSR_EDIT*0.01)+"m";
        ui->LINSHIXIANSUCHANGDU->setText(temp_str);
        temp_str.clear();
        temp_str.append(QString::number(RTD.Balish_ID >> (3+14)));
        temp_str.push_back('-');
        temp_str.append(QString::number((RTD.Balish_ID>>14) & 7));
        temp_str.push_back('-');
        temp_str.append(QString::number(RTD.Balish_ID&0x3FFF >> 8));
        temp_str.push_back('-');
        temp_str.append(QString::number(RTD.Balish_ID & 0xFF));
        ui->YINGDAQIBIANHAO->setText(temp_str);
    }
    else
    {
        ui->CHECHANG->setText("无数据");
        ui->ZUIDACHETOUWUCHA->setText("无数据");
        ui->ZUIXIAOCHETOUWUCHA->setText("无数据");
        ui->ZIZOUXINGJULI->setText("无数据");
        ui->YUNXUSUDU->setText("无数据");
        ui->LINSHIXIANSUCHANGDU->setText("无数据");
        ui->YINGDAQIBIANHAO->setText("无数据");
        ui->MACHANGDU->setText("无数据");
    }

}

QVector<QString> MainWindow::AnalysisDMS_RTD2String(const RealTimeDatastructure& RTD)
{
	QVector<QString> ret;
	MEANINGLESS(RTD.ATPTimestamp) ? ret.push_back("ATP时间:无法提供") : ret.push_back("ATP时间:" + QDateTime::fromMSecsSinceEpoch(RTD.ATPTimestamp).toString("yyyy-MM-dd hh:mm:ss"));
	MEANINGLESS(RTD.IPCTimestamp) ? ret.push_back("接收时间:无法提供") : ret.push_back("接收时间:" + QDateTime::fromMSecsSinceEpoch(RTD.IPCTimestamp).toString("yyyy-MM-dd hh:mm:ss"));
	if (MEANINGLESS(RTD.MAINT_VIT_TRAIN_LCD))
	{
		ret.push_back("线路速度:无法提供");
	}
	MEANINGLESS(RTD.MAINT_VIT_TRAIN_LCD) ? ret.push_back("线路速度:无法提供") : ret.push_back("线路速度:" + QString::number(RTD.MAINT_VIT_TRAIN_LCD));
	MEANINGLESS(RTD.MAINT_KV_VIT_CTRL_FU) ? ret.push_back("EBP速度:无法提供") : ret.push_back("EBP速度:" + QString::number(RTD.MAINT_KV_VIT_CTRL_FU));
	MEANINGLESS(RTD.MAINT_VIT_PRE_INTERV_LCD) ? ret.push_back("NBP速度:无法提供") : ret.push_back("NBP速度:" + QString::number(RTD.MAINT_VIT_PRE_INTERV_LCD));
	MEANINGLESS(RTD.MAINT_VIT_OBJECTIF_LCD) ? ret.push_back("目标速度:无法提供") : ret.push_back("目标速度:" + QString::number(RTD.MAINT_VIT_OBJECTIF_LCD));
	QString temp_str;
	temp_str.resize(8);
	temp_str[0] = QChar(char(RTD.Car_ID >> 56));
	temp_str[1] = QChar(char(RTD.Car_ID >> 48));
	temp_str[2] = QChar(char(RTD.Car_ID >> 40));
	temp_str[3] = QChar(char(RTD.Car_ID >> 32));
	temp_str[4] = QChar(char(RTD.Car_ID >> 24));
	temp_str[5] = QChar(char(RTD.Car_ID >> 16));
	temp_str[6] = QChar(char(RTD.Car_ID >> 8));
	temp_str[7] = QChar(char(RTD.Car_ID >> 0));
	MEANINGLESS(RTD.MAINT_VIT_OBJECTIF_LCD) ? ret.push_back("车次:无法提供") : ret.push_back("车次:" + temp_str);
	switch (RTD.CTCS_LEVEL_EDIT)
	{
	case 0:
		temp_str = "CTCS-0";
		break;
	case 1:
		temp_str = "CTCS-1";
		break;
	case 2:
		temp_str = "CTCS-2";
		break;
	case 3:
		temp_str = "CTCS-3";
		break;
	case 4:
		temp_str = "CTCS-4";
		break;
	default:
		temp_str = "预期外的值";
	}
	MEANINGLESS(RTD.CTCS_LEVEL_EDIT) ? ret.push_back("列控等级:无法提供") : ret.push_back("列控等级:" + temp_str);
	if (RTD.CTCS_MODE_EDIT > sizeof(DMS_DONGZUOMOSHI_LIST) / sizeof(DMS_DONGZUOMOSHI_LIST[0]))
		temp_str = "预期外的值";
	else
		temp_str = DMS_DONGZUOMOSHI_LIST[RTD.CTCS_MODE_EDIT];
	MEANINGLESS(RTD.CTCS_MODE_EDIT) ? ret.push_back("控车模式:无法提供") : ret.push_back("控车模式:" + temp_str);
	switch (RTD.BREAK_COMMAND)
	{
	case 0:
		temp_str = "无";
		break;
	case 1:
		temp_str = "SB1[一级常用制动]";
		break;
	case 4:
		temp_str = "SB4[四级常用制动]";
		break;
	case 7:
		temp_str = "SB7[七级常用制动]";
		break;
	case 13:
		temp_str = "EB[紧急制动]";
		break;
	default:
		temp_str = "预期外的值";
	}
	MEANINGLESS(RTD.BREAK_COMMAND) ? ret.push_back("制动模式:无法提供") : ret.push_back("制动模式:" + temp_str);
	temp_str = QString::number(RTD.MAINT_DIS_OBJECTIF) + "m";
	MEANINGLESS(RTD.BREAK_COMMAND) ? ret.push_back("目标距离:无法提供") : ret.push_back("目标距离:" + temp_str);
	temp_str = QString::number(RTD.MAINT_POS_GEO_EDIT) + "m";
	MEANINGLESS(RTD.BREAK_COMMAND) ? ret.push_back("里程:无法提供") : ret.push_back("里程:" + temp_str);
	if (RTD.CTCS_CODE_EDIT >= sizeof(DMS_ZAIPIN_LIST) / sizeof(DMS_ZAIPIN_LIST[0]))
		temp_str = "预期外的值";
	else
		temp_str = DMS_ZAIPIN_LIST[RTD.CTCS_CODE_EDIT];
	MEANINGLESS(RTD.CTCS_CODE_EDIT) ? ret.push_back("载频:无法提供") : ret.push_back("载频:" + temp_str);
	if (RTD.CTCS_CARRIER_EDIT >= sizeof(DMS_DIPIN_LIST) / sizeof(DMS_DIPIN_LIST[0]))
		temp_str = "预期外的值";
	else
		temp_str = DMS_DIPIN_LIST[RTD.CTCS_CARRIER_EDIT];
	MEANINGLESS(RTD.CTCS_CARRIER_EDIT) ? ret.push_back("低频:无法提供") : ret.push_back("低频:" + temp_str);
	temp_str = QString::number(RTD.CTCS_DRIVER_ID);
	MEANINGLESS(RTD.CTCS_DRIVER_ID) ? ret.push_back("司机ID:无法提供") : ret.push_back("司机ID:" + temp_str);
	return ret;
}

void MainWindow::UpdateBaliseCompareResult(RealTimeDatastructure &RTD)
{
    resultVec.clear();
    resultVec = RTD.Compare_Result;
    if (resultVec[0].size())
    {
        UpdateTabWidget_udp(resultVec[0], C_YINGDAQIWEIZHI, RTD.IPCTimestamp);
        m_balise_location_list.push_back(resultVec[0]);
    }
    if(resultVec[1].size())
    {
        UpdateTabWidget_udp(resultVec[1], C_XIANLUPODU, RTD.IPCTimestamp);
        m_grade_list.push_back(resultVec[1]);
    }
    if (resultVec[2].size())
    {
        UpdateTabWidget_udp(resultVec[2], C_XIANLUSUDU, RTD.IPCTimestamp);
        m_speed_list.push_back(resultVec[2]);
    }
    if (resultVec[3].size())
    {
        UpdateTabWidget_udp(resultVec[3], C_FENXIANGQU, RTD.IPCTimestamp);
        m_neutral_section_list.push_back(resultVec[3]);
    }
    if (resultVec[4].size())
    {
        UpdateTabWidget_udp(resultVec[4], C_CHEZHAN, RTD.IPCTimestamp);
        m_track_station_list.push_back(resultVec[4]);
    }
    if (resultVec[5].size())
    {
        UpdateTabWidget_udp(resultVec[5], C_GUIDAODIANLU, RTD.IPCTimestamp);
        m_track_circuit_list.push_back(resultVec[5]);
    }
    if (resultVec[6].size())
    {
        UpdateTabWidget_udp(resultVec[6], C_LINSHIXIANSU, RTD.IPCTimestamp);
        m_temporary_speed_list.push_back(resultVec[6]);
    }

}



/***********************************************槽函数***********************************************************/

/**********************************************************
* @功能：显示实时比对结果的槽函数，此函数负责添加比对历史，如有错，在首页添加错误信息，并建立比对历史和原始数据的映射表，以及错误信息与比对历史的映射表
* @形参：result-用于绘制表格的原始数据（固定格式： result[0][0]存放应答器标识号 result[0][1]存放原始数据索引 result[1]存放比对项（应答器)
*       result[2]存放比对项（表）result[3]存放比对结果，以此顺序类推）；compareType-比对类型，用于判断在哪个比对页显示信息
* @返回值：无
* @备注：信号与槽连接见main.cpp
* @作者：hb
* @修改日期：
**********************************************************/
void MainWindow::UpdateTabWidget_udp(const QVector<QVector<QString> > &result, CompareType compareType, qint64 timepoint)
{
    QString errorStr = "";
    QListWidget *listWidget_temp = NULL;
    QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间
    QString timeStr = time.toString("yyyy-MM-dd hh:mm:ss"); //设置显示格式
    QString list_name = result[0][0];
    switch (compareType)
    {
    case C_YINGDAQIWEIZHI:
        listWidget_temp = ui->listWidget_baliseLocation_compare_history;
        errorStr = timeStr + "     应答器链接错误";
        break;
    case C_GUIDAODIANLU:
        listWidget_temp = ui->listWidget_trackCircuit_compare_history;
        errorStr = timeStr + "     码序错误";
        break;
    case C_XIANLUSUDU:
        listWidget_temp = ui->listWidget_speed_compare_history;
        errorStr = timeStr + "     线路速度不一致";
        break;
    case C_XIANLUPODU:
        listWidget_temp = ui->listWidget_grade_compare_history;
        errorStr = timeStr + "     线路坡度错误";
        break;
    case C_CHEZHAN:
        listWidget_temp = ui->listWidget_station_compare_history;
        errorStr = timeStr + "     车站或站台侧信息错误";
        break;
    case C_FENXIANGQU:
        listWidget_temp = ui->listWidget_neutralSection_compare_history;
        errorStr = timeStr + "     分相区信息错误";
        break;
    case C_LINSHIXIANSU:
        listWidget_temp = ui->listWidget_temporySpeed_history;
        break;
    default: break;
    }

    //**********添加比对历史**********************************
    //将应答器编号添加到比对历史栏
    QListWidgetItem* listWidgetItem = new QListWidgetItem(
                list_name, listWidget_temp);
    int count = listWidget_temp->count();
    listWidget_temp->insertItem(listWidget_temp->count(), listWidgetItem);
    timePointMap[compareType].push_back(timepoint);
    bool hasError = false;
    if (compareType == C_YINGDAQIWEIZHI || compareType == C_CHEZHAN )
    {
        if (result[0][1] == "Wrong")
            hasError = true;
    }
    else
    {
        for (int i = 0; i < result.size(); i++)
        {
            for (int j = 0; j < result[i].size(); j++)
            {
                if (result[i][j].contains("不一致") || result[i][j] == "无匹配")
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

    if (hasError)
    {
        listWidgetItem->setTextColor(Qt::red);
        //首页显示错误
        QListWidgetItem *listItem = new QListWidgetItem(errorStr);
        listItem->setBackgroundColor(QColor(255, 0, 0));
        ui->Errmsg_LISTWIDGET->addItem(listItem);
        ui->Errmsg_LISTWIDGET->scrollToBottom();//每次滚动到最下方
        //建立首页错误信息和tab页面比对历史的映射
        m_errorHistoryVec.append(listWidget_temp->count() - 1);

        switch (compareType)
        {
        case C_YINGDAQIWEIZHI:
            m_e_balise_location_list.append(result);
            break;
        case C_GUIDAODIANLU:
            m_e_track_circuit_list.append(result);
            break;
        case C_XIANLUSUDU:
            m_e_speed_list.append(result);
            break;
        case C_XIANLUPODU:
            m_e_grade_list.append(result);
            break;
        case C_LICHENG:
            break;
        case C_CHEZHAN:
            m_e_track_station_list.append(result);
            break;
        case C_FENXIANGQU:
            m_e_neutral_section_list.append(result);
            break;
        default: break;
        }
    }
}

/**********************************************************
* @功能：点击比对历史，显示比对结果的槽函数
* @形参：result-比对结果 compareType-compareType-比对类型，用于判断在哪个比对页绘制表格
* @返回值：
* @备注：信号与槽连接见main.cpp,列数及名称需在mainwindow.ui中指定
* @作者：hb
* @修改日期：
**********************************************************/
void MainWindow::UpdateTabWidget_click(const QVector<QVector<QString> > &result, CompareType compareType)
{
    if (result.size() == 0 || result.size() == 1)
	{
		return;
	}
	QTableWidget *tableWidget_temp = NULL;
	switch (compareType)
	{
	case C_YINGDAQIWEIZHI:
		tableWidget_temp = ui->tableWidget_baliseLocation_compare_info;
		break;
	case C_GUIDAODIANLU:
		tableWidget_temp = ui->tableWidget_trackCircuit_compare_info;
		break;
	case C_XIANLUSUDU:
		tableWidget_temp = ui->tableWidget_speed_compare_info;
		break;
	case C_XIANLUPODU:
		tableWidget_temp = ui->tableWidget_grade_compare_info;
		break;
	case C_LICHENG:
		break;
	case C_CHEZHAN:
        tableWidget_temp = ui->tableWidget_station_compare_info;
		break;
	case C_FENXIANGQU:
		tableWidget_temp = ui->tableWidget_neutralSection_compare_info;
		break;
	case C_LINSHIXIANSU:
		tableWidget_temp = ui->tableWidget_temporySpeed_info;
		break;
	default:
		break;
	}
	tableWidget_temp->clearContents(); //清空上一次绘制的表格内容
	//设置最后一列充满整个表格其余部分
	//tableWidget_temp->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	//设置行数
	//int row_count = tableWidget_temp->rowCount();
	//tableWidget_temp->setColumnCount(result[1].size());
	//tableWidget_temp->setEditTriggers(QAbstractItemView::NoEditTriggers);

	//填充表格, 注意result[0]放的是应答器标识号和原始数据索引
	//qDebug() << "colCount" << tableWidget_temp->columnCount();
	//qDebug() << "rowCount" << tableWidget_temp->rowCount();
	if (compareType == C_GUIDAODIANLU || compareType == C_XIANLUSUDU || compareType == C_XIANLUPODU || compareType == C_FENXIANGQU || compareType == C_CHEZHAN || compareType == C_YINGDAQIWEIZHI)
	{
		tableWidget_temp->horizontalHeader()->setStyleSheet("QHeaderView::section{background:green;}");
		tableWidget_temp->verticalHeader()->setStyleSheet("QHeaderView::section{background:green;}");
		tableWidget_temp->horizontalHeader()->setDefaultSectionSize(tableWidget_temp->width() / 6);
		//tableWidget_temp->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
		if (compareType != C_YINGDAQIWEIZHI && compareType != C_CHEZHAN)
			tableWidget_temp->setRowCount(result.size() - 3);
		else
			tableWidget_temp->setRowCount(result.size() - 2);
        tableWidget_temp->setColumnCount(result[0].size());
		QStringList header;
		int header_index = 1;
		if (compareType != C_YINGDAQIWEIZHI && compareType != C_CHEZHAN)//[0] = 069-1-16-010-2
            header_index++;
        for(int i=1;i< result[header_index].size();++i)
            header << result[header_index][i];
		header_index++;
		tableWidget_temp->setHorizontalHeaderLabels(header);
		header.clear();
        for (int i = header_index; i < result.size(); ++i)
            header << result[i][0];
		tableWidget_temp->setVerticalHeaderLabels(header);
		tableWidget_temp->verticalHeader()->show();
        tableWidget_temp->setColumnCount(result[header_index].size()-1);
        for(int i = header_index; i < result.size(); i++)
        {
            for(int j=1;j<result[i].size();++j)
            {
                QString content = result[i][j];
                bool errflag = false;

                if(content.endsWith('W')||content.endsWith('R'))
                {
                    if(content.endsWith('W'))
                        errflag = true;
                    content.resize(content.size()-1);
                }
                if(content.contains("不一致")||content.contains("无匹配"))
                    errflag = true;
                tableWidget_temp->setItem(i- header_index, j-1, new QTableWidgetItem(content));
                if(errflag)
                    tableWidget_temp->item(i - header_index, j-1)->setBackgroundColor(QColor(Qt::red));
            }
        }
    }
}




/**********************************************************
* @功能：主界面点击错误信息的槽函数
* @形参：
* @返回值：
* @备注：
* @作者：hb
* @修改日期：
**********************************************************/
void MainWindow::on_Errmsg_LISTWIDGET_clicked(const QModelIndex &index)
{
    if (index.row() < m_errorHistoryVec.size())
    {
        if (index.data().toString().contains("应答器链接错误"))
        {
            ui->listWidget_baliseLocation_compare_history->setCurrentRow(m_errorHistoryVec[index.row()]);
            ui->tabWidget->setCurrentIndex(TAB_POSITION_YINGDAQIWEIZHI);
            UpdateTabWidget_click(m_balise_location_list[m_errorHistoryVec[index.row()]], C_YINGDAQIWEIZHI);
        }
        if (index.data().toString().contains("分相区信息错误"))
        {
            ui->listWidget_neutralSection_compare_history->setCurrentRow(m_errorHistoryVec[index.row()]);
            ui->tabWidget->setCurrentIndex(TAB_POSITION_FENXINGQU);
            UpdateTabWidget_click(m_neutral_section_list[m_errorHistoryVec[index.row()]], C_FENXIANGQU);  //向比较函数发送信号
        }
        if (index.data().toString().contains("线路速度不一致"))
        {
            ui->listWidget_speed_compare_history->setCurrentRow(m_errorHistoryVec[index.row()]);
            ui->tabWidget->setCurrentIndex(TAB_POSITION_XIANLUSUDU);
            UpdateTabWidget_click(m_speed_list[m_errorHistoryVec[index.row()]], C_XIANLUSUDU);  //向比较函数发送信号
        }
        if (index.data().toString().contains("线路坡度错误"))
        {
            ui->listWidget_grade_compare_history->setCurrentRow(m_errorHistoryVec[index.row()]);
            ui->tabWidget->setCurrentIndex(TAB_POSITION_XIANLUPODU);
            UpdateTabWidget_click(m_grade_list[m_errorHistoryVec[index.row()]], C_XIANLUPODU);  //向比较函数发送信号
        }
        if (index.data().toString().contains("车站或站台侧信息错误"))
        {
            ui->listWidget_station_compare_history->setCurrentRow(m_errorHistoryVec[index.row()]);
            ui->tabWidget->setCurrentIndex(TAB_POSITION_CHEZHAN);
            UpdateTabWidget_click(m_track_station_list[m_errorHistoryVec[index.row()]], C_CHEZHAN);  //向比较函数发送信号
        }
        if (index.data().toString().contains("码序错误"))
        {
            ui->listWidget_trackCircuit_compare_history->setCurrentRow(m_errorHistoryVec[index.row()]);
            ui->tabWidget->setCurrentIndex(TAB_POSITION_GUIDAODIANLU);
            UpdateTabWidget_click(m_track_circuit_list[m_errorHistoryVec[index.row()]], C_GUIDAODIANLU);  //向比较函数发送信号
        }
    }
}

void MainWindow::on_listWidget_trackCircuit_compare_history_clicked(const QModelIndex &index)
{
    UpdateTabWidget_click(m_track_circuit_list[index.row()], C_GUIDAODIANLU);
}

void MainWindow::on_listWidget_neutralSection_compare_history_clicked(const QModelIndex &index)
{
    UpdateTabWidget_click(m_neutral_section_list[index.row()], C_FENXIANGQU);
}

void MainWindow::on_listWidget_baliseLocation_compare_history_clicked(const QModelIndex &index)
{
    UpdateTabWidget_click(m_balise_location_list[index.row()], C_YINGDAQIWEIZHI);
}

void MainWindow::on_listWidget_speed_compare_history_clicked(const QModelIndex &index)
{
    UpdateTabWidget_click(m_speed_list[index.row()], C_XIANLUSUDU);
}

void MainWindow::on_listWidget_grade_compare_history_clicked(const QModelIndex &index)
{
    UpdateTabWidget_click(m_grade_list[index.row()], C_XIANLUPODU);
}

void MainWindow::on_listWidget_station_compare_history_clicked(const QModelIndex &index)
{
    UpdateTabWidget_click(m_track_station_list[index.row()], C_CHEZHAN);
}

void MainWindow::on_listWidget_temporySpeed_history_clicked(const QModelIndex &index)
{
    UpdateTabWidget_click(m_temporary_speed_list[index.row()], C_LINSHIXIANSU);
}

void MainWindow::on_Config_arg_triggered()
{
}

void MainWindow::AnalysisInfoClick_SLOT(qint64 pos)//当视图中的重要信息被点击时用于展示点击信息
{
	RealTimeDatastructure RTD;
	ChartView::getDataFromFile(pos, &RTD);
	if (RTD.Has_Balish_Compare_Result)
		Change_AnalysisResult(&RTD.BalishData,nullptr);
	else if(RTD.Has_DMS_RBC_DATA)
		Change_AnalysisResult(nullptr, &RTD.RBC);
}

void MainWindow::Change_AnalysisResult(BaliseBitMessage* BBM,QByteArray* RBC)
{
    if(BBM!=nullptr)//显示应答器信息
    {
        RBCS_DIALOG->ChangeAnalyzeResult(BBM);
        Analyze_BaliseMessage::Resolve(*BBM);
        ui->AnalysisResult_treeWidget->clear();
        ui->AnalysisTitle_label->setText("应答器:"+Analyze_BaliseMessage::GetBaliseID(*BBM));
        ui->AnalysisResult_treeWidget->setColumnCount(3);//设置treewidget控件为三列表示
        ui->AnalysisResult_treeWidget->setHeaderLabels(QStringList()<<"包名/项名"<<"十进制表示（十六进制表示值）"<<"解析结果");//设置每一列的标题
        /********************************将解析内容反映到treewidget控件上**********************/
        QList<QTreeWidgetItem*> temp_treeitem;
        for(int i=0;i<Analyze_BaliseMessage::Analyze_Result.size();i++)
        {
            ui->AnalysisResult_treeWidget->addTopLevelItem(new QTreeWidgetItem(QStringList(Analyze_BaliseMessage::Analyze_Result[i][0].name)));
            for(int j=1;j<Analyze_BaliseMessage::Analyze_Result[i].size();j++)
            {
                temp_treeitem.push_back(new QTreeWidgetItem(QStringList()<<Analyze_BaliseMessage::Analyze_Result[i][j].name<<QString::number(Analyze_BaliseMessage::Analyze_Result[i][j].value,10)+tr(" (0x")+QString::number(Analyze_BaliseMessage::Analyze_Result[i][j].value,16)+tr(")")<<Analyze_BaliseMessage::Analyze_Result[i][j].remark));
            }
            ui->AnalysisResult_treeWidget->topLevelItem(i)->addChildren(temp_treeitem);
            //设置鼠标悬停的提示，以便信息显示不全的时候可以通过人工鼠标悬停的方式查看完整信息
            for(int j=1;j<Analyze_BaliseMessage::Analyze_Result[i].size();j++)
            {
                ui->AnalysisResult_treeWidget->topLevelItem(i)->child(j-1)->setToolTip(2,Analyze_BaliseMessage::Analyze_Result[i][j].remark);
            }
            temp_treeitem.clear();
        }
        ui->AnalysisResult_treeWidget->topLevelItem(0)->setExpanded(true);
        QString BinaryMessage;
        for(unsigned int i=0;i<sizeof(BBM->Balise_Tel)/sizeof(BBM->Balise_Tel[0]);++i)
        {
            BinaryMessage.append(QString("%1").arg((unsigned char)BBM->Balise_Tel[i],2,16,QChar('0')).toUpper()+" ");
        }
        ui->BinaryData_textBrowser->setText(BinaryMessage);
    }
    else if(RBC!=nullptr)//显示RBC信息
    {
        RBCS_DIALOG->ChangeAnalyzeResult(RBC);
        PackCollectorRBC PCR = Analyze_RBCMessage::Resolve(*RBC);
        if(PCR[0].size()==1)
            qDebug()<<"Resolve failed.";
        ui->AnalysisResult_treeWidget->clear();
        ui->AnalysisTitle_label->setText("RBC:"+PCR[0][0].remark);
        ui->AnalysisResult_treeWidget->setColumnCount(3);//设置treewidget控件为三列表示
        ui->AnalysisResult_treeWidget->setHeaderLabels(QStringList()<<"包名/项名"<<"十进制表示（十六进制表示值）"<<"解析结果");//设置每一列的标题
        /********************************将解析内容反映到treewidget控件上**********************/
        QList<QTreeWidgetItem*> temp_treeitem;
        for(int i=0;i<PCR.size();i++)
        {
            ui->AnalysisResult_treeWidget->addTopLevelItem(new QTreeWidgetItem(QStringList(PCR[i][0].remark)));
            for(int j=1;j<PCR[i].size();j++)
            {
                temp_treeitem.push_back(new QTreeWidgetItem(QStringList()<<PCR[i][j].name<<QString::number(PCR[i][j].value,10)+tr(" (0x")+QString::number(PCR[i][j].value,16)+tr(")")<<PCR[i][j].remark));
            }
            ui->AnalysisResult_treeWidget->topLevelItem(i)->addChildren(temp_treeitem);
            //设置鼠标悬停的提示，以便信息显示不全的时候可以通过人工鼠标悬停的方式查看完整信息
            for(int j=1;j<PCR[i].size();j++)
            {
                ui->AnalysisResult_treeWidget->topLevelItem(i)->child(j-1)->setToolTip(2,PCR[i][j].remark);
            }
            temp_treeitem.clear();
        }
        ui->AnalysisResult_treeWidget->topLevelItem(0)->setExpanded(true);
        QString BinaryMessage;
        for(unsigned int i=0;i<RBC->size();++i)
        {
            BinaryMessage.append(QString("%1").arg((unsigned char)RBC->at(i),2,16,QChar('0')).toUpper()+" ");
        }
        ui->BinaryData_textBrowser->setText(BinaryMessage);
    }
}

void MainWindow::on_toolButton_clicked()
{
    RBCS_DIALOG->show();
}

void MainWindow::on_GetRepotr_MENU_triggered()
{
    GR_DIALOG->show();
}

void MainWindow::on_SetTrainNumber_MENU_triggered()
{
    QString str = QInputDialog::getText(this,"输入框","请输入车次号");
    tcpCommandClient->send(Combine_Command_Data(TcpHead(CMD_FROM::CLIENT,CMD_TYPE::CONTROL,CMD_NAME::SET_TRAIN_NUMBER),str));
}

void MainWindow::on_ClearLocal_triggered()//清空本机数据
{
	//清空界面的显示信息
	ui->Draw_FRAME->clear();
	//时间信息
	ui->ATPSHIJIANDATE->display(0);
	ui->ATPSHIJIANTIME->display(0);
	//速度信息
	ui->DANGQIANSUDU->display(0);
	ui->EBPSUDU->display(0);
	ui->NBPSUDU->display(0);
	ui->MUBIAOSUDU->display(0);
	//ATP基础数据
	ui->CHECI->clear();
	ui->DENGJI->clear();
	ui->DONGZUOMOSHI->clear();
	ui->ZHIDONGZHILING->clear();
	ui->MUBIAOJULI->clear();
	ui->LICHENG->clear();
	ui->ZAIPIN->clear();
	ui->DIPIN->clear();
	ui->SIJIID->clear();
	//DMI文本
	ui->DMIWENBEN->clear();
	//200C数据
	ui->CHECHANG->clear();
	ui->ZUIDACHETOUWUCHA->clear();
	ui->ZUIXIAOCHETOUWUCHA->clear();
	ui->ZIZOUXINGJULI->clear();
	ui->YUNXUSUDU->clear();
	ui->LINSHIXIANSUCHANGDU->clear();
	ui->YINGDAQIBIANHAO->clear();
	ui->MACHANGDU->clear();
	//其他
	ui->Errmsg_LISTWIDGET->clear();
	ui->BinaryData_textBrowser->clear();
	ui->AnalysisResult_treeWidget->clear();
	ui->AnalysisTitle_label->setText("无数据");
	//数据清除工作
	//清楚历史错误记录
	m_errorHistoryVec.clear();
	//比对结果存储表
	m_balise_location_list.clear();
	m_track_circuit_list.clear();
	m_speed_list.clear();
	m_grade_list.clear();
	m_track_station_list.clear();
	m_neutral_section_list.clear();
	m_temporary_speed_list.clear();
	//错误结果存储表
	m_e_balise_location_list.clear();
	m_e_track_circuit_list.clear();
	m_e_speed_list.clear();
	m_e_grade_list.clear();
	m_e_track_station_list.clear();
	m_e_neutral_section_list.clear();
	m_e_temporary_speed_list.clear();
	//点击和比对信息界面的信息删除
	ui->listWidget_baliseLocation_compare_history->clear();
	ui->tableWidget_baliseLocation_compare_info->clear();
	ui->tableWidget_trackCircuit_compare_info->clear();
	ui->listWidget_trackCircuit_compare_history->clear();
	ui->tableWidget_speed_compare_info->clear();
	ui->listWidget_speed_compare_history->clear();
	ui->tableWidget_grade_compare_info->clear();
	ui->listWidget_grade_compare_history->clear();
	ui->listWidget_station_compare_history->clear();
	ui->tableWidget_station_compare_info->clear();
	ui->tableWidget_neutralSection_compare_info->clear();
	ui->listWidget_neutralSection_compare_history->clear();
	ui->tableWidget_temporySpeed_info->clear();
	ui->listWidget_temporySpeed_history->clear();
    //清除map对应的信息
    timePointMap.clear();
}

void MainWindow::on_ImportATPFiles_MENU_triggered()
{
    QStringList fileNameList = QFileDialog::getOpenFileNames(
                this, "导入列控数据", "", "EXCEL (*.xls *.xlsx)");
    if(fileNameList.isEmpty())
    {
        QMessageBox::information(this,"提示","没有选择文件！");
        return;
    }
    //表格数据处理
    QMessageBox processBox;
    processBox.setIcon(QMessageBox::Information);
    processBox.setText("正在处理数据表，请勿操作");
    processBox.setStandardButtons(QMessageBox::NoButton);
    processBox.show();
    QApplication::processEvents();
    {
        QByteArray QBA;
        QDataStream QDS(&QBA, QIODevice::WriteOnly);
        QDS.setVersion(QDataStream::Qt_5_12);
        QDS<<fileNameList;
        QFile file(QApplication::applicationDirPath()+"/filelist.datastream");
        file.open(QIODevice::WriteOnly);
        file.write(QBA);
        file.close();
    }
    QProcess excelConverter;
    QString path  = QApplication::applicationDirPath()+"/ExcelConverter.exe";
    excelConverter.start(path);
    if (!excelConverter.waitForStarted())
    {
        processBox.close();
        QMessageBox::information(this,"数据表导入失败","失败原因:ExcelConverter.exe启动失败");
        return;
    }
    if (!excelConverter.waitForFinished(-1))
    {
        QMessageBox::information(this,"数据表导入失败","失败原因:ExcelConverter.exe执行错误");
        return;
    }
    QFile dataSheet(QApplication::applicationDirPath() + '/' + "sheet.datastream");
    if (!dataSheet.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(this,"数据表导入失败","失败原因:sheet.datastream文件读取失败");
        return;
    }
    processBox.close();
    QApplication::processEvents();
    QByteArray QBA = dataSheet.readAll();
    QString result;
    QDataStream QDS(&QBA, QIODevice::ReadOnly);
    QDS.setVersion(QDataStream::Qt_5_12);
    QDS>>result;
    if(result=="ERROR")
    {
        QDS>>result;
        QMessageBox::critical(this,"数据表导入失败","失败原因:"+result);
    }
    else if(result=="OK")
    {
        QDS.startTransaction();
        QDS >> result;
        QDS >> DesignData::accessRodeMap >> DesignData::stationVec >> DesignData::gradeDownProVec >> DesignData::gradeUpProVec\
                        >> DesignData::gradeDownBackVec >> DesignData::gradeUpBackVec >> DesignData::neutralSectionUpVec >> DesignData::neutralSectionDownVec\
                        >> DesignData::pathWayDataDownProVec >> DesignData::pathWayDataUpProVec >> DesignData::pathWayDataDownBackVec >> DesignData::pathWayDataUpBackVec\
                        >> DesignData::pathWaySpeedDownVec >> DesignData::pathWaySpeedUpVec >> DesignData::pathWaySpeedDownBackVec >> DesignData::pathWaySpeedUpBackVec\
                        >> DesignData::baliseLocationUpMap >> DesignData::baliseLocationDownMap >> DesignData::balishUseMap >> DesignData::brokenLinkVec >> DesignData::stationSideVec;
        if(!QDS.commitTransaction())
        {
            QMessageBox::critical(this,"数据转换失败","数据无效！");
            return;
        }
        QByteArray SendQBA;
        QDataStream SendStream(&SendQBA,QIODevice::WriteOnly);
        SendStream.setVersion(QDataStream::Qt_5_6);
        SendStream << DesignData::accessRodeMap << DesignData::stationVec << DesignData::gradeDownProVec << DesignData::gradeUpProVec\
                        << DesignData::gradeDownBackVec << DesignData::gradeUpBackVec << DesignData::neutralSectionUpVec << DesignData::neutralSectionDownVec\
                        << DesignData::pathWayDataDownProVec << DesignData::pathWayDataUpProVec << DesignData::pathWayDataDownBackVec << DesignData::pathWayDataUpBackVec\
                        << DesignData::pathWaySpeedDownVec << DesignData::pathWaySpeedUpVec << DesignData::pathWaySpeedDownBackVec << DesignData::pathWaySpeedUpBackVec\
                        << DesignData::baliseLocationUpMap << DesignData::baliseLocationDownMap << DesignData::balishUseMap << DesignData::brokenLinkVec << DesignData::stationSideVec;
        qDebug()<<"数据表大小:"<<SendQBA.size();
        //发送数据
        QMessageBox::information(this,"提示","处理结果："+result);
        QMessageBox box;
        box.setIcon(QMessageBox::Information);
        box.setText("数据表已发出，正在等待远端回应...");
        box.setStandardButtons(QMessageBox::NoButton);
        box.show();
        QApplication::processEvents();
        wait_Mutex.lock();
        tcpCommandClient->send(Combine_Command_Data(TcpHead(CMD_FROM::CLIENT, CMD_TYPE::CONTROL, CMD_NAME::DATA_SHEET), SendQBA));
        if(!wait_Condition.wait(&wait_Mutex,5000))
        {
            box.close();
            QApplication::processEvents();
            QMessageBox::critical(this,"远程未响应","数据发送失败！");
            wait_Mutex.unlock();
            return;
        }
        else
        {
            box.close();
            QApplication::processEvents();
            QMessageBox::information(this,"数据表已发出","数据发送成功！");
            wait_Mutex.unlock();
            return;
        }
    }
    else
    {
        QMessageBox::information(this,"数据表导入失败","失败原因:sheet.datastream文件内容无效");
    }
}

void MainWindow::on_ConfigUserInfo_MENU_triggered()
{
    static UserManagementUI* UMU = new UserManagementUI(UserName,this);
    UMU->refreshAllUser();
    UMU->show();
}

void MainWindow::on_GetLocalData_MENU_triggered()
{
    auto fileNameList = QFileDialog::getOpenFileNames(
                this, "本地数据", "", "DMSDATA (*.dmsCompareData)");
    if(fileNameList.size()!=1 || fileNameList.empty())
    {
        QMessageBox::information(this,"提示","输入只能为单一文件！");
        return;
    }
    QFile file;
    file.setFileName(fileNameList[0]);
    file.open(QIODevice::ReadOnly);
    if(!file.isOpen())
    {
        QMessageBox::information(this,"提示","文件读取失败！");
        return;
    }
    auto QBA = file.readAll();
    QDataStream QDS(&QBA,QIODevice::ReadOnly);
    RealTimeDatastructure RTD;
    //QMutexLocker locker(&MainWindow::m_static_mutex);
    while (!QDS.atEnd())
    {
        QDS >> RTD;
        MainWindow::RTD_Queue.push_back(RTD);
    }
    move_frame = false;
    emit tcpCommandClient->ReDraw_MainWindow_SIGNAL();
}

void MainWindow::on_IPConfig_MENU_triggered()
{
    QString new_IP = QInputDialog::getText(this,"设置连接IP","请输入要连接的IP,输入框内容为当前设置的IP:",QLineEdit::Normal,ARM_IP);
    auto sp = new_IP.split('.');
    bool input_illegal = false;
    if(sp.size()!=4)
        input_illegal = true;
    for(auto str:sp)
    {
        if(str.size()>3)
        {
            input_illegal = true;
            break;
        }
        for(auto c:str)
        {
            if(!(c>='0'&&c<='9'))
            {
                input_illegal = true;
                break;
            }
        }
        if(input_illegal)
            break;
        bool ok = false;
        int num = str.toInt(&ok);
        if(!ok)
        {
            input_illegal = true;
            break;
        }
        if(!(num>=0&&num<=255))
        {
            input_illegal = true;
            break;
        }
    }
    if(input_illegal)
    {
        QMessageBox::information(this,"提示","输入IP有误");
        return;
    }
    ARM_IP = new_IP;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    exit(0);
}

void MainWindow::on_listWidget_baliseLocation_compare_history_doubleClicked(const QModelIndex &index)
{
    ui->tabWidget->setCurrentIndex(0);
    ui->Draw_FRAME->moveCenturalToPoint(timePointMap[C_YINGDAQIWEIZHI][index.row()]);
}



void MainWindow::on_listWidget_trackCircuit_compare_history_doubleClicked(const QModelIndex &index)
{
    ui->tabWidget->setCurrentIndex(0);
    ui->Draw_FRAME->moveCenturalToPoint(timePointMap[C_GUIDAODIANLU][index.row()]);
}

void MainWindow::on_listWidget_speed_compare_history_doubleClicked(const QModelIndex &index)
{
    ui->tabWidget->setCurrentIndex(0);
    ui->Draw_FRAME->moveCenturalToPoint(timePointMap[C_XIANLUSUDU][index.row()]);
}

void MainWindow::on_listWidget_grade_compare_history_doubleClicked(const QModelIndex &index)
{
    ui->tabWidget->setCurrentIndex(0);
    ui->Draw_FRAME->moveCenturalToPoint(timePointMap[C_XIANLUPODU][index.row()]);
}

void MainWindow::on_listWidget_station_compare_history_doubleClicked(const QModelIndex &index)
{
    ui->tabWidget->setCurrentIndex(0);
    ui->Draw_FRAME->moveCenturalToPoint(timePointMap[C_CHEZHAN][index.row()]);
}

void MainWindow::on_listWidget_neutralSection_compare_history_doubleClicked(const QModelIndex &index)
{
    ui->tabWidget->setCurrentIndex(0);
    ui->Draw_FRAME->moveCenturalToPoint(timePointMap[C_FENXIANGQU][index.row()]);
}

void MainWindow::on_listWidget_temporySpeed_history_doubleClicked(const QModelIndex &index)
{
    ui->tabWidget->setCurrentIndex(0);
    ui->Draw_FRAME->moveCenturalToPoint(timePointMap[C_LINSHIXIANSU][index.row()]);
}

void MainWindow::show_VersionError(QString err)
{
    QMessageBox::information(this,"版本不一致",err);
    exit(0);
}
