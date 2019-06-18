#include "InnerTcpSocket.h"
#include "qprocess.h"
#include "WLAN_TcpServer.h"
extern WLAN_TcpServer* g_wlanTcpServer;

InnerTcpSocket::InnerTcpSocket(qintptr handle,InnerTcpServer* server):
    TcpAbstract(handle,5000)
{
	this->setParent(server);
	timeoutTimer.setSingleShot(true);
    connect(server, &InnerTcpServer::send, this, &InnerTcpSocket::sendArray_SLOT, Qt::QueuedConnection);


	//读取表格文件到内存
	QFile file("sheet.dat");
	if (file.exists())
	{
		file.open(QIODevice::ReadOnly);
		QByteArray QBA = file.readAll();
		this->sendArray_SLOT(Combine_Command_Data(TcpHead(CMD_FROM::RECEIVER, CMD_TYPE::INSIDE, CMD_NAME::DATA_SHEET), QBA));
		QByteArray temp = QBA.mid(0, 8);
		qDebug() << temp.toHex();
		//qDebug() << sizeof(TcpHead);
		qDebug() << "inside send sheet data size:" << QBA.size();
		file.close();
	}
	else
	{
		qDebug() << "data sheet doesn' exist.";
	}
}

InnerTcpSocket::~InnerTcpSocket()
{

}

void InnerTcpSocket::HeartbeatTimeout_SLOT()
{
#ifdef _WIN32
    qDebug()<<(QProcess::startDetached("ARM_Worker.exe")?"ARM_Worker start success.":"ARM_Worker start failed.");
#endif
#ifdef __linux
    qDebug()<<(QProcess::startDetached("/home/root/ARM_Worker")?"ARM_Worker start success.":"ARM_Worker start failed.");
#endif

	this->abort();
	this->deleteLater();
	qDebug() << "inner connection closed.";
}

void InnerTcpSocket::getNewSocketDescriptor(qintptr nsd)
{
	if (this->isOpen())
	{
		this->close();
	}
	this->setSocketDescriptor(nsd,QAbstractSocket::ConnectedState,QIODevice::ReadWrite);
	this->updateConnectionInfo();
	qDebug() << this->isOpen() << this->isWritable();
}

void InnerTcpSocket::decodeBuffer(QDataStream &ds)
{
	ds >> TH;
	if (TH.cmd_from == CMD_FROM::WORKER && TH.cmd_type == CMD_TYPE::INSIDE)
	{
		static RealTimeDatastructure RTD;
		static ReportDataStructure RDS;
		switch (TH.cmd_name)
		{
		case CMD_NAME::DMS_ATP:
		case CMD_NAME::DMS_Balish:
		case CMD_NAME::DMS_RBC:
			ds >> RTD;
			g_wlanTcpServer->sendToAll(Combine_Command_Data(TcpHead(CMD_FROM::RECEIVER, CMD_TYPE::DATA, static_cast<CMD_NAME>(TH.cmd_name)), RTD));
			DMSCompareResultFile << RTD;
			break;
		case CMD_NAME::DMS_REPORT:
			ds >> RDS;
			DMSReportFile << RDS;
			break;
		default:
			qDebug() << "invalid cmd_name.";
			assert(false);
			break;
		}
	}
	else
	{
		qDebug() << "invalid cmd_from & cmd_type.";
		assert(false);
	}
}
