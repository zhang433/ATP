#include "TcpCommandClient.h"
#include <readdesignfile_thread.h>

TcpCommandClient::TcpCommandClient():
    TcpAbstract (QHostAddress(ARM_IP),12300)
{
    //命令连接是可以发送数据的
    connect(this, &TcpCommandClient::send, this, &TcpCommandClient::sendArray_SLOT,Qt::QueuedConnection);
    connect(this, &TcpCommandClient::init, this, &TcpCommandClient::startConnect,Qt::QueuedConnection);
	connect(this, &TcpCommandClient::stateChanged, this, &TcpCommandClient::changeStatus_SLOT);
	HeartbeatTimeout_SLOT();
}

TcpCommandClient::~TcpCommandClient()
{

}

void TcpCommandClient::changeStatus_SLOT(QAbstractSocket::SocketState state)
{
	if (state == QAbstractSocket::UnconnectedState)
		emit UpdateMainWondowStatue_SIGNAL("命令:未连接", "QLabel{ color: red }", STATUS_BAR::COMMAND_SATUS);
	else if (state == QAbstractSocket::ConnectedState)
	{
		emit UpdateMainWondowStatue_SIGNAL("命令:已连接", "QLabel{ color: green }", STATUS_BAR::COMMAND_SATUS);
		HeartbeatTimeout_SLOT();
		// 连接成功后先告知对方自己的连接属性
        sendArray_SLOT(Combine_Command_Data(TcpHead(CMD_FROM::CLIENT, CMD_TYPE::CONTROL, CMD_NAME::SOCKET_TYPE), static_cast<quint8>(COMMAND_SOCKET)));
	}
}

void TcpCommandClient::decodeBuffer(QDataStream& QDS)
{
	static QString BackColor;
    QDS >> TH;
    if (TH.cmd_from == CMD_FROM::RECEIVER)//从工控机来的数据
    {
        if (TH.cmd_type == CMD_TYPE::CONTROL)
        {
            switch (TH.cmd_name)
            {
            case CMD_NAME::DATA_SHEET:
                ProcessingCommand_DATA_SHEET(QDS);
                break;
            case CMD_NAME::SEND_TRAIN_NUMBER:
            {
                QString Sequence;
                QDS >> Sequence;
                emit UpdateMainWondowStatue_SIGNAL("车次:"+Sequence, "QLabel{ color: green }",STATUS_BAR::TRAIN_SEQUENCE);
                break;
            }
            case CMD_NAME::LIST_FILE:
                ProcessingCommand_LIST_FILE(QDS);
                break;
            case CMD_NAME::GET_SELECTED_FILE:
                ProcessingCommand_GET_SELECTED_FILE(QDS);
                break;
            case CMD_NAME::BETTERY_PERSENT:
				qint8 p1;
				QDS >> p1;
				BackColor = (p1 <= 20 ? "QLabel{ color: red }" : "QLabel{ color: green }");
				emit UpdateMainWondowStatue_SIGNAL("电量:" + QString::number(p1) + "%", BackColor, STATUS_BAR::BETTERY_STATUS);
                break;
			case CMD_NAME::SD_PERSENT:
				qreal p2;
				QDS >> p2;
				BackColor = (p2 >= 80 ? "QLabel{ color: red }" : "QLabel{ color: green }");
				emit UpdateMainWondowStatue_SIGNAL("SD卡:" + QString::number(p2,'f',2)+"%", BackColor, STATUS_BAR::SD_CAPACITY);
				break;
            }
        }
        else
        {
            qDebug() << "unknow IPC data";
        }
    }
    else
    {
        qDebug() << "unknow data source:";
    }
}

void inline TcpCommandClient::ProcessingCommand_DATA_SHEET(QDataStream& ds)
{
    QString sheetName;
    QString fileName;

    ds >> fileName >> sheetName;
    if (sheetName == "clear")
    {
        emit HasReturn2Sheet_SIGNAL("clear success!");
        //return;
    }
    else
    {
        sheetMutex.lock();
        foreach (auto var,sRecordVec)
        {
            if (var.sheetName == (fileName + sheetName))
            {
                var.isReturn = true;
                emit HasReturn2Sheet_SIGNAL(fileName + " " + sheetName + "传输成功");
            }
        }
        sheetMutex.unlock();
    }
}

void inline TcpCommandClient::ProcessingCommand_LIST_FILE(QDataStream& ds)
{
    QVector<QVector<QString>> FileList;
    ds>>FileList;
    emit GetFileList_SIGNAL(FileList);
}


void inline TcpCommandClient::ProcessingCommand_GET_SELECTED_FILE(QDataStream& ds)
{
    QString arg;
    ds >> arg;
    if (arg == "report")//生成报表的服务
    {
        QVector<SequenceDataStructure> V_SDS;
        ds >> V_SDS;
        emit GetSelectedFile_SIGNAL(V_SDS);
    }
    else if (arg == "data")//回读数据服务
    {
        QByteArray QBA;
        ds >> QBA;
        QDataStream QDS(&QBA,QIODevice::ReadOnly);
        RealTimeDatastructure RTD;
        QMutexLocker locker(&MainWindow::m_static_mutex);
        while (!QDS.atEnd())
        {
            QDS >> RTD;
            MainWindow::RTD_Queue.push_back(RTD);
        }
        emit ReDraw_MainWindow_SIGNAL();
        emit changeDialogStatue_SIGNAL("green", "done");
    }
	else if (arg == "download")
	{
		QVector<QString> fileName;
		QVector<QByteArray> binData;
		ds >> fileName >> binData;
		emit GetDownloadFile_SIGNAL(fileName, binData);
	}
}

void TcpCommandClient::startConnect()
{
    this->keepTryingConnectUntilSuccess();
}

void TcpCommandClient::HeartbeatTimeout_SLOT()
{
    this->sendArray_SLOT(heartBeatBuffer);//此处有可能会在socket连接关闭后执行
}

