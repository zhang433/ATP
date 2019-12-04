#include "wlan_tcpsocket.h"
#include "InnerTcpServer.h"
#include <QStorageInfo>
#include <QDir>
#include <QtAlgorithms>

extern QString EXE_VERSION;

WLAN_TcpSocket::WLAN_TcpSocket(qintptr socketDescriptor, WLAN_TcpServer* server):
    TcpAbstract(socketDescriptor,5000),
    statusTimer(this)
{
	timeoutTimer.setSingleShot(true);
    this->setParent(server);
    //connect(this,&WLAN_TcpSocket::disconnected,this,&WLAN_TcpSocket::deleteLater);//关闭连接的时候直接销毁本对象，当再来新连接的时候会创建新的对象
}

WLAN_TcpSocket::~WLAN_TcpSocket()
{

}

void WLAN_TcpSocket::HeartbeatTimeout_SLOT()
{
    qDebug() << connectionInfo + " HeartBeat timeout.";
	this->abort();
    QCoreApplication::processEvents();
    this->deleteLater();
    //this->thread()->deleteLater();
}

void WLAN_TcpSocket::decodeBuffer(QDataStream &in)
{
    TcpHead TH;
    in >> TH;
    if (TH.cmd_from == CMD_FROM::CLIENT&&TH.cmd_type == CMD_TYPE::CONTROL)
    {
        switch (TH.cmd_name)
        {
        case CMD_NAME::DATA_SHEET://数据表
            ProcessingCommand_DATASHEET(in);
            break;
        case CMD_NAME::LIST_FILE: //列出所有列表
            ProcessingCommand_LIST_FILE(in);
            break;
        case CMD_NAME::GET_SELECTED_FILE: //获取选定文件
            ProcessingCommand_GET_SELECTED_FILE(in);
            break;
		case CMD_NAME::SOCKET_TYPE://初始化时设置的socket类型
			ProcessingCommand_SOCKET_TYPE(in);
			break;
        case CMD_NAME::VERSION://索要版本号
            this->sendArray_SLOT(Combine_Command_Data(TcpHead(CMD_FROM::RECEIVER, CMD_TYPE::CONTROL, CMD_NAME::VERSION),EXE_VERSION));
            break;
        case CMD_NAME::SET_TRAIN_NUMBER://设定车次
        {
            QString str;
            in >> str;
            assert(DMSReportFile.name().contains('/'));//假设路径必为子文件夹
            qDebug() << str;
            DMSReportFile.reName(str);
            DMSCompareResultFile.reName(str);
            DMSBit_File.reName(str);

            int pos1 = DMSReportFile.name().lastIndexOf('/');
            int pos2 = DMSReportFile.name().lastIndexOf('.');
            QString TrainNumber = DMSReportFile.name().mid(pos1 + 1, pos2 - pos1 - 1);
            this->sendArray_SLOT(Combine_Command_Data(TcpHead(CMD_FROM::RECEIVER, CMD_TYPE::CONTROL, CMD_NAME::SEND_TRAIN_NUMBER), TrainNumber));
            qDebug() << "send train number";
            break;
        }
        }
    }
}

void inline WLAN_TcpSocket::ProcessingCommand_LIST_FILE(QDataStream& in)
{
    QString rootPath;
    in>>rootPath;

#ifdef __linux
    QDir dir(rootPath);
#elif WIN32
    QDir dir(g_fileRootPath);
#endif
    dir.setFilter(QDir::Dirs |  QDir::NoDotAndDotDot);
	QList<QFileInfo> fileInfo = dir.entryInfoList();

	QVector<QVector<QString>> FileList;//每个QVector<QString>,第0项是文件夹目录，第1项-n项是该文件夹目录下的文件目录
	for (auto iter = fileInfo.begin(); iter != fileInfo.end(); iter++)
	{
		QVector<QString> DirList;
		DirList.push_back(iter->baseName());
		qDebug() << iter->absoluteFilePath();
		QDir dir(iter->absoluteFilePath());
		dir.setFilter(QDir::Files);
		QList<QFileInfo> fileInfo_InPerDir = dir.entryInfoList();
		for (auto iter2 = fileInfo_InPerDir.begin(); iter2 != fileInfo_InPerDir.end(); iter2++)
			DirList.push_back(iter2->fileName());
		FileList.push_back(DirList);
	}
    this->sendArray_SLOT(Combine_Command_Data(TcpHead(CMD_FROM::RECEIVER, CMD_TYPE::CONTROL, CMD_NAME::LIST_FILE), FileList));
}

void inline WLAN_TcpSocket::ProcessingCommand_DATASHEET(QDataStream& in)
{
    qDebug()<<"receive sheet data.";
    QByteArray QBA;
    QFile file("sheet.dat");
    in>>QBA;
    if(file.open(QIODevice::WriteOnly))
    {
        file.write(QBA);
        file.close();
    }
    inner_tcp->send(Combine_Command_Data(TcpHead(CMD_FROM::RECEIVER, CMD_TYPE::INSIDE, CMD_NAME::DATA_SHEET), QBA));
    this->sendArray_SLOT(Combine_Command_Data(TcpHead(CMD_FROM::RECEIVER, CMD_TYPE::CONTROL, CMD_NAME::SHEETRECEIVED_REPLY)));
}

void WLAN_TcpSocket::SendBetteryPersent_SLOT()
{
#ifdef __linux
    static int count_times = 0;
    if(count_times%1==0)
    {
        //static QByteArray QBA(4,0);
        //static QFile file("/sys/bus/iio/devices/iio\\:device0/in_voltage0_raw");
        static QVector<qreal> table = { 12,13.62,13.81,13.96,14.1,14.2,14.27,14.32,14.39,14.43,14.5,14.59,14.7,14.85,15.01,15.19,15.36,15.54,15.71,15.89,16.1 };
        static qreal sum_voltage = 0.0;
        //if (file.exists())
        {
//            if (!file.isOpen())
//                file.open(QIODevice::ReadOnly);
//            file.seek(0);
//            file.read(QBA.data(), 4);
//            int adc_val = QString(QBA).toInt();
            int adc_val = 3300;
            qreal voltage = (adc_val/4095.0*3.3)/(1.91/(1.91+8.06));
            sum_voltage += voltage;
            qint8 bettery_persent;
            if(count_times == 0)
            {
                auto iter = qUpperBound(table.begin(), table.end(), sum_voltage);
                if (iter == table.end())
                    bettery_persent = 100;
                else
                    bettery_persent = static_cast<qint8>(5 * (iter - table.begin()));
                this->sendArray_SLOT(Combine_Command_Data(TcpHead(CMD_FROM::RECEIVER, CMD_TYPE::CONTROL, CMD_NAME::BETTERY_PERSENT), bettery_persent));
                sum_voltage=0;
            }
            else if (count_times!=0 && count_times%5==0)
            {
                sum_voltage /= 5;
                auto iter = qUpperBound(table.begin(), table.end(), sum_voltage);
                if (iter == table.end())
                    bettery_persent = 100;
                else
                    bettery_persent = static_cast<qint8>(5 * (iter - table.begin()));
                this->sendArray_SLOT(Combine_Command_Data(TcpHead(CMD_FROM::RECEIVER, CMD_TYPE::CONTROL, CMD_NAME::BETTERY_PERSENT), bettery_persent));
                sum_voltage = 0;
            }
        }
    }
    if(count_times%30==0)
    {
        QStorageInfo storage("/mnt");
        storage.refresh();
        if(storage.rootPath()=="/mnt")
        {
            qreal usedPersent = (1 - static_cast<qreal>(storage.bytesAvailable())/storage.bytesTotal())*100;
            this->sendArray_SLOT(Combine_Command_Data(TcpHead(CMD_FROM::RECEIVER, CMD_TYPE::CONTROL, CMD_NAME::SD_PERSENT), usedPersent));
            qDebug()<<storage.bytesAvailable()/1000000000<<"GB"<<usedPersent;
        }
    }
    count_times++;
#endif
}

void inline WLAN_TcpSocket::ProcessingCommand_GET_SELECTED_FILE(QDataStream& in)
{
    QString in_arg;
    in >> in_arg;
    QVector<QString> v_path;
    in >> v_path;
    if (in_arg == "data")
	{
        QString arg("data");
		QFile file;
		ReportFile_MTX.lock();
        file.setFileName(g_fileRootPath + "/" + v_path[0]);
		file.open(QIODevice::ReadOnly);
        assert(file.isOpen());
		QByteArray file_data = file.readAll();
		ReportFile_MTX.unlock();
        this->sendArray_SLOT(Combine_Command_Data(TcpHead(CMD_FROM::RECEIVER, CMD_TYPE::CONTROL, CMD_NAME::GET_SELECTED_FILE), arg, file_data));
	}
    else if(in_arg == "report")
	{
		QString arg("report");

		QVector<SequenceDataStructure> V_SDS;
		SequenceDataStructure SDS;
		for (auto path : v_path)
		{
			SDS.sequenceNumber.clear();
			SDS.reportData.clear();
			QFile file;
			ReportFile_MTX.lock();
            file.setFileName(g_fileRootPath + "/" + path);
			file.open(QIODevice::ReadOnly);
            assert(file.isOpen());
			QByteArray file_data = file.readAll();
			ReportFile_MTX.unlock();

			QDataStream stream(&file_data, QIODevice::ReadOnly);
			SDS.sequenceNumber = path.mid(path.lastIndexOf('/') + 1, -1);
			while (!stream.atEnd()) {
				SDS.reportData.resize(SDS.reportData.size() + 1);
				stream >> SDS.reportData.back();
			}
			V_SDS.push_back(SDS);
		}
        this->sendArray_SLOT(Combine_Command_Data(TcpHead(CMD_FROM::RECEIVER, CMD_TYPE::CONTROL, CMD_NAME::GET_SELECTED_FILE), arg, V_SDS));
	}
    else if(in_arg == "download")
    {
        QString arg("download");
        QVector<QString> fileName;
        QVector<QByteArray> binData;
        for(auto& path:v_path)
        {
            if(QFile::exists(g_fileRootPath + "/" + path))
            {
                QFile file(g_fileRootPath + "/" + path);
                if(file.open(QIODevice::ReadOnly))
                {
                    fileName.push_back(path);
                    binData.push_back(file.readAll());
                }
            }
        }
        this->sendArray_SLOT(Combine_Command_Data(TcpHead(CMD_FROM::RECEIVER, CMD_TYPE::CONTROL, CMD_NAME::GET_SELECTED_FILE), arg, fileName,binData));
    }
    else if(in_arg == "delete")
    {
        for(auto& path:v_path)
        {
            if(QFile::exists(g_fileRootPath + "/" + path))
                QFile::remove(g_fileRootPath + "/" + path);
        }
        QDir dir(g_fileRootPath);
        dir.setFilter(QDir::Dirs |  QDir::NoDotAndDotDot);
        QList<QFileInfo> fileInfo = dir.entryInfoList();

        QVector<QVector<QString>> FileList;//每个QVector<QString>,第0项是文件夹目录，第1项-n项是该文件夹目录下的文件目录
        for (auto iter = fileInfo.begin(); iter != fileInfo.end(); iter++)
        {
            QVector<QString> DirList;
            DirList.push_back(iter->baseName());
            qDebug() << iter->absoluteFilePath();
            QDir dir(iter->absoluteFilePath());
            dir.setFilter(QDir::Files);
            QList<QFileInfo> fileInfo_InPerDir = dir.entryInfoList();
            for (auto iter2 = fileInfo_InPerDir.begin(); iter2 != fileInfo_InPerDir.end(); iter2++)
                DirList.push_back(iter2->fileName());
            FileList.push_back(DirList);
        }
        this->sendArray_SLOT(Combine_Command_Data(TcpHead(CMD_FROM::RECEIVER, CMD_TYPE::CONTROL, CMD_NAME::LIST_FILE), FileList));
    }
}

void inline WLAN_TcpSocket::ProcessingCommand_SOCKET_TYPE(QDataStream& in)
{
	quint8 socketType;
	in >> socketType;
    if (socketType == static_cast<quint8>(COMMAND_SOCKET))
	{
		qDebug() << connectionInfo + " is command socket.";
		connect(&statusTimer, &QTimer::timeout, this, &WLAN_TcpSocket::SendBetteryPersent_SLOT);//send bettery persent every senconds
		statusTimer.setInterval(1000);
		statusTimer.start();

		int pos1 = DMSReportFile.name().lastIndexOf('/');
		int pos2 = DMSReportFile.name().lastIndexOf('.');
		QString TrainNumber = DMSReportFile.name().mid(pos1 + 1, pos2 - pos1 - 1);
		qDebug() << TrainNumber;
        this->sendArray_SLOT(Combine_Command_Data(TcpHead(CMD_FROM::RECEIVER, CMD_TYPE::CONTROL, CMD_NAME::SEND_TRAIN_NUMBER), TrainNumber));
	}
    else if (socketType == static_cast<quint8>(DATA_SOCKET))
	{
		qDebug() << connectionInfo + " is data socket.";
		connect(reinterpret_cast<WLAN_TcpServer*>(parent()), &WLAN_TcpServer::sendToAll, this, &WLAN_TcpSocket::sendArray_SLOT, Qt::QueuedConnection);//TCP应用层的广播实现方式，同一个信号连接多个槽函数
	}
	else
		qDebug() << "unknown socket type.";
}
