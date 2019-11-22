#pragma once
#include "TcpAbstract.h"


class EthTcpClient:public TcpAbstract
{
    Q_OBJECT
public:
    EthTcpClient();
    ~EthTcpClient() override;
signals:
    void init();
private:
    QTimer timer;//30s一次传输“GETDATA”
    quint8 step = 0,datatype=0;
    quint16 datalength,protocol_version,crc_result;
    QByteArray DataStream,ReceiveData;
    int count_frame=0;
    char mask_begin[2] = {static_cast<char>(0xA0),static_cast<char>(0xA0)};
    char mask_invalid[8] = { static_cast<char>(0xFF),static_cast<char>(0xFF),static_cast<char>(0xFF),static_cast<char>(0xFF),static_cast<char>(0xFF),static_cast<char>(0xFF),static_cast<char>(0xFF),static_cast<char>(0xFF) };
    char mask_end[2] = {static_cast<char>(0x0A),static_cast<char>(0x0A)};
    void Read_SLOT() override;
    void decodeBuffer(QDataStream& ds) override;
    quint16 calculate_crc(quint16 wCRCin,quint16 wCPoly,quint16 wResultXOR,bool input_invert,bool ouput_invert,const char *puchMsg, int usDataLen);//计算crc的值
private slots:
    void startConnect();
    void HeartbeatTimeout_SLOT() override;
};
