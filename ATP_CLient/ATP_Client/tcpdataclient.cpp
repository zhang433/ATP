#include "tcpdataclient.h"
#include <readdesignfile_thread.h>

TcpDataClient::TcpDataClient():
    TcpAbstract (QHostAddress(ARM_IP),12300)
{
    //���ӳɹ����ȸ�֪�Է��Լ�����������
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
		emit UpdateMainWondowStatue_SIGNAL("����:δ����", "QLabel{ color: red }", STATUS_BAR::DATA_STATUS);
	else if (state == QAbstractSocket::ConnectedState)
	{
		emit UpdateMainWondowStatue_SIGNAL("����:������", "QLabel{ color: green }", STATUS_BAR::DATA_STATUS);
		//���ӳɹ����ȸ�֪�Է��Լ�����������
		HeartbeatTimeout_SLOT();
        sendArray_SLOT(Combine_Command_Data(TcpHead(CMD_FROM::CLIENT, CMD_TYPE::CONTROL, CMD_NAME::SOCKET_TYPE), static_cast<quint8>(DATA_SOCKET)));
	}
}

void TcpDataClient::decodeBuffer(QDataStream& QDS)
{
	QDS >> TH;
    if (TH.cmd_from == CMD_FROM::RECEIVER)//�ӹ��ػ���������
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
					MainWindow::RTD_Queue.push_back(RTD);//�̼߳��첽ִ�У�ʹ�ö���ȷ�����ݻ�ȡ˳��
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
    this->sendArray_SLOT(heartBeatBuffer);//�˴��п��ܻ���socket���ӹرպ�ִ��
}

void TcpDataClient::startConnect()
{
    this->keepTryingConnectUntilSuccess();
}
