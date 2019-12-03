#include <QCoreApplication>
#include "eth_udp.h"
#include "self_define.h"
#include <fstream>
#include <QFile>
#include <QDataStream>
#include <QProcess>
#include <QString>
#include <ethtcpclient.h>
#include <qdir.h>
#include <QDateTime>
#include <InnerTcpSocket.h>
#include <QStorageInfo>
#include "WLAN_TcpServer.h"

QString EXE_VERSION = "0.9.0";

HistoryFile ATPBit_File, DMSBit_File;
WLAN_TcpServer* g_wlanTcpServer;
InnerTcpServer* inner_tcp;
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
#ifdef __linux
    //check Sdcard has enough space(must>5%)
    {
        QStorageInfo storage("/mnt");
        storage.refresh();
        if(storage.rootPath()!="/mnt")
            qDebug()<<"/mnt not mounted.";
        else
        {
            qreal usedPersent = (1 - storage.bytesAvailable()*1.0/storage.bytesTotal())*100;
            qDebug()<<"SD Card size:"<<QString::number(storage.bytesTotal()/1000000000.0)+"GB";
            qDebug()<<"available size:"<<QString::number(storage.bytesAvailable()/1000000000.0)+"GB";
            qDebug()<<"used SD Card:"<<QString::number(usedPersent)+"%";
            if(usedPersent>=95)//remove ealierest file record
            {
                QDir dir("/mnt");
                QStringList files = dir.entryList({}, QDir::Dirs|QDir::Writable|QDir::NoDotAndDotDot, QDir::Name);
                for(auto iter = files.begin();iter!=files.end();iter++)
                {
                    if(!dir.remove(dir.path()+'/'+*iter))
                    {
                        qDebug()<<"remove dir"<<dir.path()+'/'+*iter<<"failed,check permission.";
                        break;
                    }
                    storage.refresh();
                    usedPersent = (1 - static_cast<qreal>(storage.bytesAvailable())/storage.bytesTotal())*100;
                    if(usedPersent<95)
                        break;
                }
            }
        }
    }
#endif

    {
        QDir qDir;
        g_fileRootPath = qDir.exists("/mnt")?"/mnt":QCoreApplication::applicationDirPath();
        if (!qDir.exists(g_fileRootPath + "/ReportFile"))
            qDebug() << (qDir.mkdir(g_fileRootPath + "/ReportFile") == true ? "ReportFile文件夹创建成功" : "ReportFile文件夹创建失败");
        else
            qDebug() << ("ReportFile文件夹已存在");

        QString TimeDir = QDateTime::currentDateTime().toString("yyyy-MM-dd");
        g_fileRootPath += "/ReportFile";
        TimeDir = g_fileRootPath + "/" + TimeDir;
        if (qDir.exists(TimeDir) == true)
            qDebug() << (TimeDir + "已存在");
        else
            qDir.mkdir(TimeDir);

        QString FilePath = TimeDir + "/Default" + QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
        DMSReportFile.SetFilePath(FilePath + ".dmsSummary");
        DMSCompareResultFile.SetFilePath(FilePath + ".dmsCompareData");
        DMSBit_File.SetFilePath(FilePath + ".dmsBinData");

        QProcess QP;
        QP.start(QString("ifconfig"));
        QP.waitForFinished();
        QString ret_str = QP.readAllStandardOutput();
        if(ret_str.contains("eth0"))
        {
            //config eth1 static ip to receive udp package from 172.168.15.24
            //system("ifconfig eth1 up");
            system("ifconfig eth0 192.168.1.100 netmask 255.255.255.0");
            qDebug()<<"eth0 config to 192.168.1.100.";
        }
        else
            qDebug()<<"eth1 already ready,skip Initlization.";
    }

    //内部进程间通信的udp连接
    QThread* inner_tcp_THREAD = new QThread();
    inner_tcp = new InnerTcpServer();
    inner_tcp->moveToThread(inner_tcp_THREAD);
    inner_tcp_THREAD->start();

    //有线的DMStcp线程，DMS数据
    QThread* DMSTcp_THREAD = new QThread();
    EthTcpClient* eth_tcp = new EthTcpClient();
    eth_tcp->moveToThread(DMSTcp_THREAD);
    DMSTcp_THREAD->start();
    emit eth_tcp->init();

    //Udp监听线程，ATP数据
    //new ETH_Udp(inner_Tcp);

    QThread* WLANTcp_THREAD = new QThread();
    g_wlanTcpServer = new WLAN_TcpServer();
    g_wlanTcpServer->moveToThread(WLANTcp_THREAD);
    WLANTcp_THREAD->start();

    return a.exec();
}
