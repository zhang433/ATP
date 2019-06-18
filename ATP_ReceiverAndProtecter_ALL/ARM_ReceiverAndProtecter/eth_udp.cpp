#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include "eth_udp.h"
#include "qdebug.h"
#include <iostream>
#include <QDateTime>
QMutex ETH_Udp::MTX;


//QVector<ATPData> ETH_Udp::ATPBit_Vector;//接收到的历史数据
//QVector<BaliseBitMessage> ETH_Udp::BaliseBit_Vector;

unsigned long long Receive_Count = 0;
unsigned long long Receive_Count4 = 0;


/* prototype of the packet handler */



ETH_Udp::ETH_Udp(InnerTcpServer* inner_udp)
{
    this->inner_udp = inner_udp;
    this->bind(QHostAddress::AnyIPv4,32100);
    connect(this,SIGNAL(readyRead()),this,SLOT(readPendingDatagrams()));
    qDebug()<<"ETH_UDP is waiting for ATP data on 32100 port of all address.";
}

ETH_Udp::~ETH_Udp()
{

}

QVector<unsigned char> ETH_Udp::GetAtpBitMsg_InBinFile(QFile& file, int order)
{
    QVector<unsigned char> ret;
    ret.resize(Frame_Size);
    MTX.lock();
    if (!file.isOpen())//检测文件是否被打开了
    {
        exit(-3);//未打开时程序异常退出，报告错误代码-3
    }
    auto current_pos = file.pos();
    if(!file.seek(Frame_Size * order))
    {
        exit(-3);//未打开时程序异常退出，报告错误代码-3
    }
    file.read((char*)&ret[0], Frame_Size);
    file.seek(current_pos);
    MTX.unlock();
    return ret;
}

void ETH_Udp::readPendingDatagrams()
{
    while(this->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(this->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        this->readDatagram(datagram.data(), datagram.size(),&sender, &senderPort);
        {
            if (datagram.size() == 1413)
            {
                if (++frame_count == 4)
                {
                    ATPBit_File.writeQBA(datagram);
                    QByteArray QBA;
                    QDataStream QDS(QBA);
                    QDS<<datagram;
                    emit inner_udp->send(QBA);//发送数据，在工作进程里面进行比对
                }
                Receive_Count++;
            }
            else if (datagram.size() == 13)
            {
                if (frame_count != 8)
                    std::cout << "Error!frame_count is " << frame_count <<std::endl;
                Receive_Count++;
                frame_count = 0;
            }
            else
            {
                std::cout << "package length illegal:" << datagram.size() <<std::endl;
            }
        }
    }
}
