#include "TcpCommandClient.h"

extern QString REMOTE_VERSION;

TcpCommandClient::TcpCommandClient():
    TcpAbstract (QHostAddress(ARM_IP),12300)
{
    //????????????
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
        sendArray_SLOT(Combine_Command_Data(TcpHead(CMD_FROM::CLIENT, CMD_TYPE::CONTROL, CMD_NAME::SOCKET_TYPE), static_cast<quint8>(COMMAND_SOCKET)));
	}
}

void TcpCommandClient::decodeBuffer(QDataStream& QDS)
{
	static QString BackColor;
    QDS >> TH;
    if (TH.cmd_from == CMD_FROM::RECEIVER)
    {
        if (TH.cmd_type == CMD_TYPE::CONTROL)
        {
            switch (TH.cmd_name)
            {
            case CMD_NAME::VERSION:
            {
                QString remote_version;
                QDS >> remote_version;
                REMOTE_VERSION = remote_version;
                MainWindow::wait_Mutex.lock();
                MainWindow::wait_Condition.notify_all();
                MainWindow::wait_Mutex.unlock();
                break;
            }
            case CMD_NAME::SHEETRECEIVED_REPLY:
                MainWindow::wait_Mutex.lock();
                MainWindow::wait_Condition.notify_all();
                MainWindow::wait_Mutex.unlock();
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
				emit UpdateMainWondowStatue_SIGNAL("空间使用:" + QString::number(p2,'f',2)+"%", BackColor, STATUS_BAR::SD_CAPACITY);
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
    if (arg == "report")//???????
    {
        QVector<SequenceDataStructure> V_SDS;
        ds >> V_SDS;
        emit GetSelectedFile_SIGNAL(V_SDS);
    }
    else if (arg == "data")//??????
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
    this->sendArray_SLOT(heartBeatBuffer);//???????socket???????
}

