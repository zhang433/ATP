#include "ethtcpclient.h"
#include <QDebug>
#include <Command.h>
#include <iostream>
#include "InnerTcpServer.h"

EthTcpClient::EthTcpClient():
#ifdef WIN32
	TcpAbstract(QHostAddress("127.0.0.1"), 20121, 5000)
#else
	TcpAbstract(QHostAddress("192.168.1.106"), 20121, 5000)
#endif
{
    connect(this, &EthTcpClient::init, this, &EthTcpClient::startConnect,Qt::QueuedConnection);
}

void EthTcpClient::HeartbeatTimeout_SLOT()
{
    this->sendArray_SLOT("GETDATA");
}

EthTcpClient::~EthTcpClient()
{
}

void EthTcpClient::Read_SLOT()
{
    ReceiveData = this->readAll();
    DataStream.append(ReceiveData);
    DMSBit_File.writeQBA(ReceiveData);
    while(1)
    {
        if(step==0)//表示还没有找到同步字节
        {
            if(DataStream.size()<2)
                return;
            int aim_index=-1;
            for(int i=0;i<=DataStream.size()-static_cast<int>(sizeof(mask_begin));++i)
            {
                if(memcmp(DataStream.data()+i,mask_begin,sizeof(mask_begin))==0)
                {
                    aim_index=i;
                    break;
                }
            }
            if(aim_index>=0)//表示找到了同步字节
            {
                DataStream.remove(0,aim_index+2);
                step++;
            }
            else//没有找到的话就保留最后一个字节，因为最后一个字节很可能是0xa0
            {
                char temp = DataStream[DataStream.size()-1];
                DataStream.clear();
                DataStream.push_back(temp);
                return;
            }
        }
        enum CMD_NAME cmd_name = CMD_NAME::UNUSED_NAME;
        if(step==1)//提取数据包长度
        {
            if(DataStream.size()<2)
                return;
            datalength = static_cast<quint16>((quint16(DataStream[1])<<8)+static_cast<quint8>(DataStream[0]-2));//待修改
            step++;
        }
        if(step==2)//等待收集足够长度的信息，如果收集够了，则做相应的处理
        {
            if(DataStream.size()<datalength)//待修改
            {
                //qDebug()<<"第"<<count_frame+1<<"次接收的数据中DataStream的长度不足，长度为："<<DataStream.size()+2<<"包的长度为:"<<datalength+2;
                //qDebug()<<"本次的接收数据为：";
                for(auto iter=DataStream.begin();iter!=DataStream.end();iter++)
                {
                    std::cout<<QString("%1").arg(static_cast<quint8>(*iter),2,16,QChar('0')).toStdString()<<" ";
                }
                std::cout<<std::endl;
                return;
            }
            //qDebug()<<++count_frame;
            protocol_version = static_cast<quint16>((DataStream[3])<<8)+static_cast<quint8>(DataStream[2]);//提取协议版本//待修改
            datatype = static_cast<quint8>(DataStream[4]);//提取数据标识
            Q_ASSERT(datatype>0&&datatype<4);
            int index=5;
            QByteArray DataFromStream;
            switch(datatype)
            {
            case 1://ATP基础数据，共41个字节
            {
                cmd_name = CMD_NAME::DMS_ATP;
                DataFromStream = DataStream.mid(5,41);
                index+=41;
                break;
            }
            case 2://应答器数据，共112字节
            {
                cmd_name = CMD_NAME::DMS_Balish;
                //static int count_yingdaqi=0;
                //qDebug()<<"balish"<<++count_yingdaqi;
                DataFromStream = DataStream.mid(5,112);
                index+=112;
                break;
            }
            case 3://RBC数据，共11+描述的数据长度
            {
                cmd_name = CMD_NAME::DMS_RBC;
                static int count_RBC=0;
                qDebug()<<"RBC"<<++count_RBC;
                index+=9;
                int totle_RBC_length = 11+(quint16(DataStream[index+1])<<8)+static_cast<quint8>(DataStream[index]);//待修改
                DataFromStream = DataStream.mid(5,totle_RBC_length);
                index+=totle_RBC_length-9;
                break;
            }
            default:
                //qDebug()<<"未知数据";
                break;
            }
            crc_result = static_cast<quint16>((DataStream[index+1])<<8)+static_cast<quint8>(DataStream[index]);//待修改
            quint16 my_calculate = calculate_crc(0,0x1021,0,false,false,DataStream.data(),index);
            if(my_calculate!=crc_result)
            {
                //qDebug()<<"第"<<count_frame<<"次数据接受时CRC校验错误！";
                //qDebug()<<"计算出的校验结果为："<<QString("%1").arg((my_calculate),4,16,QChar('0'));
                //qDebug()<<"数据中的校验结果为："<<QString("%1").arg((crc_result),4,16,QChar('0'));
            }
            index+=2;
            if(memcmp(DataStream.data()+index,mask_end,sizeof(mask_end))!=0)//表示同步结束标志对齐了
            {
                //qDebug()<<"未找到数据尾同步帧";
                DataStream.remove(0,index);
                step = 0;
                continue;
            }
            DataStream.remove(0,index+2);
            step = 0;
            emit inner_tcp->send(Combine_Command_Data(TcpHead(CMD_FROM::RECEIVER,CMD_TYPE::INSIDE,cmd_name),DataFromStream));//发送数据，在工作进程里面进行比对
        }
    }
}

void EthTcpClient::decodeBuffer(QDataStream &)
{
    return;
}
/*************************************************
Function:       calculate_crc
Description:    通用的16位CRC校验算法
Input:          wCRCin：CRC16算法的初始值
                wCPoly：特征多项式
                wResultXOR：结果异或值
                input_invert：输入值是否反转
                ouput_invert：输出值是否反转
                puchMsg：开始校验的数据的起始地址
                usDataLen：校验的数据长度
Output:         无输出
Return:         16位CRC校验结果
Others:         example：CRC-16/CCITT由本函数实现则填充参数如下：
                calculate_crc(0,0x1021,0,true,true,puchMsg,usDataLen)
*************************************************/
quint16 EthTcpClient::calculate_crc(quint16 wCRCin, quint16 wCPoly, quint16 wResultXOR, bool input_invert, bool ouput_invert, const char *puchMsg, int usDataLen)
{
    quint8 wChar = 0;
    while (usDataLen--)
    {
        wChar = static_cast<quint8>(*(puchMsg++));
        if(input_invert)//输入值反转
        {
            quint8 temp_char = wChar;
            wChar=0;
            for(int i=0;i<8;++i)
            {
                if(temp_char&0x01)
                    wChar|=0x01<<(7-i);
                temp_char>>=1;
            }
        }
        wCRCin ^= (wChar << 8);
        for (int i = 0; i < 8; i++)
        {
            if (wCRCin & 0x8000)
                wCRCin = static_cast<quint16>((wCRCin << 1) ^ wCPoly);
            else
                wCRCin = static_cast<quint16>(wCRCin << 1);
        }
    }
    if(ouput_invert)
    {
        quint16 temp_short = wCRCin;
        wCRCin=0;
        for(int i=0;i<16;++i)
        {
            if(temp_short&0x01)
                wCRCin|=0x01<<(15-i);
            temp_short>>=1;
        }
    }
    return (wCRCin^wResultXOR);
}

void EthTcpClient::startConnect()
{
    qDebug()<<"try to connect to DMS Bord,port 20121.";
    this->keepTryingConnectUntilSuccess();
}
