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
        ip(QHostAddress::Any),//����¼�ĵ�ַ�������IPʱ����ʾ���socket���Ӳ�����һ�������������ӵ�Client
        tcpDataStream(this)
    {
        this->setSocketDescriptor(socketDescriptor);//����socket������Ϣ
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
        if(this->waitForConnected())//ע��˲������ܵ����̵߳�����
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
    virtual void decodeBuffer(QDataStream& QDS) = 0;//������빤��
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
    virtual void HeartbeatTimeout_SLOT() = 0;//��ͬ�Ķ�ʱ���в�ͬ����;��������״̬��ʾ���������������ͣ�������������ʱ���
    void sendArray_SLOT(QByteArray QBA) {//�˴������Ǽ����ˣ���˲����̰߳�ȫ�����ǳ���socket���ӵ������߼�Ӧ�ù���һ���̣߳���˻��������ڳ�ͻ�Ŀ���
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
        timeoutTimer.setInterval(interval);//����������ʱʱ��
        connect(&timeoutTimer, &QTimer::timeout, this, &TcpAbstract::HeartbeatTimeout_SLOT);//��ʱִ�еĺ���
        connect(this, &QAbstractSocket::stateChanged, this, &TcpAbstract::StateChanged_SLOT,Qt::QueuedConnection);//��socket����״̬�ı�ʱִ�еĲۺ���
        connect(this, SIGNAL(readyRead()), this, SLOT(Read_SLOT()));//�������ݽ����źŵĲۺ���
    }
private slots:
    void StateChanged_SLOT()//socket����״̬�ĺ���
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
    virtual void Read_SLOT()//TCP�ķְ�����߼�
    {
        static qint32 expectedDataLength = 0;
        if (expectedDataLength != 0)//��δ��ȡ��ɵ�����
        {
            if (this->bytesAvailable() < expectedDataLength)//��Ȼ���߱���ȡ����
                return;
            decodeBuffer(tcpDataStream);//���л�����ִ����Ӧ����
            expectedDataLength = 0;
        }
        qint32 header;
        while (true) {
            //��ȡ����ճ��һ�������������
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
            //ʣ�໺�����ĳ��ȹ��ͽ����������͵���һ�����ݵĵ���
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
