#pragma once
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>
#include <QDebug>
#include <QDataStream>
#include <QTimer>
#include <assert.h>
#include <TcpAbstract.h>
#include <InnerTcpServer.h>
#include <Command.h>

class InnerTcpServer;

class InnerTcpSocket : public TcpAbstract
{
    Q_OBJECT
public:
    InnerTcpSocket(qintptr handle,InnerTcpServer* server);
    ~InnerTcpSocket() override;
    void getNewSocketDescriptor(qintptr socketDescriptor);
signals:
    void send(QByteArray QBA);
private:
	TcpHead TH;
	void decodeBuffer(QDataStream& ds) override;
private slots:
	void HeartbeatTimeout_SLOT() override;
};
