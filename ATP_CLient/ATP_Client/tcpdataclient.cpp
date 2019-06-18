#include "tcpdataclient.h"
#include <readdesignfile_thread.h>

TcpDataClient::TcpDataClient():
    TcpAbstract (QHostAddress(ARM_IP),12300)
{
    //连接成功后先告知对方自己的连接属性
    connect(this, &TcpDataClient::init, this, &TcpDataClient::startConnect,Qt::QueuedConnection);
	connect(this, &TcpDataClient::stateChanged, this, &TcpDataClient::changeStatus_SLOT,Qt::QueuedConnection);
	HeartbeatTimeout_SLOT();
}

TcpDataClient::~TcpDataClient()
{
}

void TcpDataClient::changeStatus_SLOT(QAbstractSocket::SocketState state)
{
	if (state == QAbstractSocket::UnconnectedState)
		emit UpdateMainWondowStatue_SIGNAL("数据:未连接", "QLabel{ color: red }", STATUS_BAR::DATA_STATUS);
	else if (state == QAbstractSocket::ConnectedState)
	{
		emit UpdateMainWondowStatue_SIGNAL("数据:已连接", "QLabel{ color: green }", STATUS_BAR::DATA_STATUS);
		//连接成功后先告知对方自己的连接属性
		HeartbeatTimeout_SLOT();
        sendArray_SLOT(Combine_Command_Data(TcpHead(CMD_FROM::CLIENT, CMD_TYPE::CONTROL, CMD_NAME::SOCKET_TYPE), static_cast<quint8>(DATA_SOCKET)));
	}
}

void TcpDataClient::decodeBuffer(QDataStream& QDS)
{
	QDS >> TH;
    if (TH.cmd_from == CMD_FROM::RECEIVER)//从工控机来的数据
	{
		if (TH.cmd_type == CMD_TYPE::DATA)
		{
			switch (TH.cmd_name)
			{
			case CMD_NAME::REALTIME_DATA:
			case CMD_NAME::DMS_ATP:
			case CMD_NAME::DMS_Balish:
			case CMD_NAME::DMS_RBC:
				QDS >> RTD;
				{
					QMutexLocker locker(&MainWindow::m_static_mutex);
					MainWindow::RTD_Queue.push_back(RTD);//线程间异步执行，使用队列确保数据获取顺序
				}
				emit ReDraw_MainWindow_SIGNAL();
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
		assert(false);
    }
}

void TcpDataClient::HeartbeatTimeout_SLOT()
{
    this->sendArray_SLOT(heartBeatBuffer);//此处有可能会在socket连接关闭后执行
}

void TcpDataClient::startConnect()
{
    this->keepTryingConnectUntilSuccess();
}
