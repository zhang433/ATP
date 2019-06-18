#pragma once
#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif
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

