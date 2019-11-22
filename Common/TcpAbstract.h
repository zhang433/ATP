#pragma once
#include <qtcpsocket.h>
#include <QDebug>
#include <qdatastream.h>
#include <qtimer.h>
#include <assert.h>
#include <qhostaddress.h>
#include <QString>
#include <QHostAddress>
#include <qmutex.h>
#include <QThread>

class TcpAbstract :public QTcpSocket
{
    Q_OBJECT
public:
    TcpAbstract(qintptr socketDescriptor,int interval = 1000):
        timeoutTimer(this),
        connectionInfo("no connection"),
        ip(QHostAddress::Any),//当记录的地址是任意的IP时，表示这个socket连接并非是一个主动发起连接的Client
        tcpDataStream(this)
    {
        this->setSocketDescriptor(socketDescriptor);//传递socket描述信息
        updateConnectionInfo();
        Init(interval);
        timeoutTimer.start();
    }
    TcpAbstract(QHostAddress address,quint16 port,int interval = 1000) :
        timeoutTimer(this),
        connectionInfo("no connection"),
        ip(address),
        port(port),
        tcpDataStream(this)
    {
        Init(interval);
    }
    TcpAbstract(int interval = 1000) :
        timeoutTimer(this),
        connectionInfo("no connection"),
        ip(QHostAddress::Any),
        tcpDataStream(this)
    {
        Init(interval);
    }
    void keepTryingConnectUntilSuccess()
    {
        assert(ip!=QHostAddress::Any);
        this->connectToHost(ip,port);
        if(this->waitForConnected())//注意此操作可能导致线程的阻塞
        {
            updateConnectionInfo();
            qDebug()<<connectionInfo+" connection has been build.";
        }
        QThread::currentThread()->sleep(2);
    }
    virtual ~TcpAbstract(){}
protected:
    QTimer timeoutTimer;
    QString connectionInfo;
    QByteArray heartBeatBuffer;
    virtual void decodeBuffer(QDataStream& QDS) = 0;//命令解码工作
    void updateConnectionInfo()
    {
        connectionInfo.clear();
        connectionInfo += "(";
        QString address = this->localAddress().toString();
        connectionInfo += address.mid(address.lastIndexOf(':') + 1) + ":";
        connectionInfo += QString::number(this->localPort()) + ",";
        address = this->peerAddress().toString();
        connectionInfo += address.mid(address.lastIndexOf(':') + 1) + ":";
        connectionInfo += QString::number(this->peerPort()) + ")";
        qDebug() << "connection info:" << connectionInfo;
    }
protected slots:
    virtual void HeartbeatTimeout_SLOT() = 0;//不同的定时器有不同的用途。可以是状态显示，可以是心跳发送，可以是心跳超时检测
    void sendArray_SLOT(QByteArray QBA) {//此处不考虑加锁了，因此并非线程安全，但是除了socket连接的其余逻辑应该共用一个线程，因此基本不存在冲突的可能
        if (this->isOpen() && this->isWritable())
        {
            //QMutexLocker locker(&sendLock);
            this->write(QBA);
        }
        else
        {
            qDebug() << connectionInfo << "sendArray_SLOT function failed, cause device can't be written.";
        }
    }
private:
    QHostAddress ip;
    quint16 port;
    qint32 socket_data_length = 0;
    QByteArray DataBuffer;
    QDataStream tcpDataStream;
    QMutex sendLock;
    void Init(int interval)
    {
        QDataStream QDS(&heartBeatBuffer,QIODevice::WriteOnly);
        QDS.setVersion(QDataStream::Qt_5_6);
        QDS << 0xAAAAAAAA;

        tcpDataStream.setVersion(QDataStream::Qt_5_6);
        timeoutTimer.setInterval(interval);//设置心跳超时时间
        connect(&timeoutTimer, &QTimer::timeout, this, &TcpAbstract::HeartbeatTimeout_SLOT);//超时执行的函数
        connect(this, &QAbstractSocket::stateChanged, this, &TcpAbstract::StateChanged_SLOT,Qt::QueuedConnection);//当socket连接状态改变时执行的槽函数
        connect(this, SIGNAL(readyRead()), this, SLOT(Read_SLOT()));//连接数据接受信号的槽函数
    }
private slots:
    void StateChanged_SLOT()//socket连接状态的函数
    {
        static enum QAbstractSocket::SocketState lastState = QAbstractSocket::HostLookupState;
        if (this->state() == QAbstractSocket::UnconnectedState)
        {
            timeoutTimer.stop();
            this->disconnectFromHost();
            this->abort();
            if(ip!=QHostAddress::Any)
            {
                if(state()!=lastState)
                    qDebug()<<connectionInfo+" connection is trying to be rebuild.";
                keepTryingConnectUntilSuccess();
            }
        }
        else if (this->state() == QAbstractSocket::ConnectedState)
        {
            updateConnectionInfo();
            timeoutTimer.start();
        }
        lastState = state();
    }
    virtual void Read_SLOT()//TCP的分包解包逻辑
    {
        static qint32 expectedDataLength = 0;
        if (expectedDataLength != 0)//有未读取完成的数据
        {
            if (this->bytesAvailable() < expectedDataLength)//仍然不具备读取条件
                return;
            decodeBuffer(tcpDataStream);//序列化解码执行相应操作
            expectedDataLength = 0;
        }
        qint32 header;
        while (true) {
            //读取所有粘在一起的心跳包数据
            while(true) {
                if(this->bytesAvailable()<static_cast<qint64>(sizeof(header)))
                    return;
                tcpDataStream >> header;
                if (header == static_cast<qint32>(0xAAAAAAAA))
                {
                    timeoutTimer.start();
                }
                else
                    break;
            }
            //剩余缓冲区的长度够就解析，不够就等下一次数据的到来
            if (this->bytesAvailable() < header)
            {
                expectedDataLength = header;
                return;
            }
            else {
                decodeBuffer(tcpDataStream);
            }
        }
    }
};
