#include "inner_tcp_client.h"
#include "designdata.h"

InnerTcpClient* G_innerTcpClient;

InnerTcpClient::InnerTcpClient():
	TcpAbstract(QHostAddress("127.0.0.1"),12345,500)
{
    connect(this,&InnerTcpClient::send,this,&InnerTcpClient::sendArray_SLOT,Qt::QueuedConnection);//数据发送信号的连接
}

void InnerTcpClient::HeartbeatTimeout_SLOT()
{
	sendArray_SLOT(heartBeatBuffer);
}

void InnerTcpClient::decodeBuffer(QDataStream &ds)
{
    TcpHead TH;
    ds>>TH;
    if(TH.cmd_from==CMD_FROM::RECEIVER && TH.cmd_type==CMD_TYPE::INSIDE)
    {
        QByteArray QBA;
        ds>>QBA;
        switch(TH.cmd_name)
        {
        case CMD_NAME::DMS_ATP:
            emit this->DMSDataCome_SIGNAL(QBA,1);
            break;
        case CMD_NAME::DMS_Balish:
            emit this->DMSDataCome_SIGNAL(QBA,2);
            break;
        case CMD_NAME::DMS_RBC:
            emit this->DMSDataCome_SIGNAL(QBA,3);
            break;
		case CMD_NAME::DATA_SHEET:
        {
            QMutexLocker locker(&DesignData::SheetMutex);
            DesignData::clearAll();
			qDebug() << "inside receive data size:"<<QBA.size();
            QDataStream QDS(&QBA,QIODevice::ReadOnly);
            QDS >> DesignData::accessRodeMap >> DesignData::stationVec >> DesignData::gradeDownProVec >> DesignData::gradeUpProVec\
                >> DesignData::gradeDownBackVec >> DesignData::gradeUpBackVec >> DesignData::neutralSectionUpVec >> DesignData::neutralSectionDownVec\
                >> DesignData::pathWayDataDownProVec >> DesignData::pathWayDataUpProVec >> DesignData::pathWayDataDownBackVec >> DesignData::pathWayDataUpBackVec\
                >> DesignData::pathWaySpeedDownVec >> DesignData::pathWaySpeedUpVec >> DesignData::pathWaySpeedDownBackVec >> DesignData::pathWaySpeedUpBackVec\
                >> DesignData::baliseLocationUpMap >> DesignData::baliseLocationDownMap >> DesignData::balishUseMap >> DesignData::brokenLinkVec >> DesignData::stationSideVec;
            if(QDS.atEnd())
                qDebug()<<"load sheet data.";
            break;
        }
        default:
            break;
        }
    }
    else {
        assert(false);
    }
}


