#pragma once
#include <qtcpserver.h>
#include "InnerTcpSocket.h"

class InnerTcpSocket;

class InnerTcpServer :public QTcpServer
{
	Q_OBJECT
public:
	InnerTcpServer(QObject* parent = nullptr);
    ~InnerTcpServer() override;
signals:
	void send(QByteArray);
private:
    InnerTcpSocket *innerTcpSocket;
	void incomingConnection(qintptr handle) override;
};

extern InnerTcpServer* inner_tcp;

