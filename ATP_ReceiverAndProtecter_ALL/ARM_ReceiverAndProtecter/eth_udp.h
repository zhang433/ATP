#ifndef SHOWRESPONSERMSG_THREAD_H
#define SHOWRESPONSERMSG_THREAD_H


//#include "ResponserData/showresponsermsg_dialog.h"
#include <QThread>
#include <QString>
#include <QVector>
#include <QUdpSocket>
#include <fstream>
#include <InnerTcpServer.h>

#include "self_define.h"



class ETH_Udp:public QUdpSocket
{
    Q_OBJECT
public:
    ETH_Udp(InnerTcpServer*);
    ~ETH_Udp();
    static QVector<unsigned char> GetAtpBitMsg_InBinFile(QFile& file, int order);
public slots:
    void readPendingDatagrams();
private:
    static QMutex MTX;
    int frame_count=0;//用来 标记收到的是第几帧的数据
    InnerTcpServer* inner_udp;
};

#endif // SHOWRESPONSERMSG_THREAD_H
