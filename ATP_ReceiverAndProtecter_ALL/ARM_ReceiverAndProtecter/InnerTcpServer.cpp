#include "InnerTcpServer.h"
#include "QProcess"
InnerTcpServer::InnerTcpServer(QObject* parent):
	innerTcpSocket(nullptr)
{
	this->setParent(parent);
    this->listen(QHostAddress("127.0.0.1"),12345);
    qDebug()<<"InnerTcpServer is listening on 127.0.0.1:12345";
#ifdef _WIN32
    qDebug()<<(QProcess::startDetached("ARM_Worker.exe")?"ARM_Worker start success.":"ARM_Worker start failed.");
#endif
#ifdef __linux
    qDebug()<<(QProcess::startDetached("/home/root/ARM_Worker")?"ARM_Worker start success.":"ARM_Worker start failed.");
#endif
}

InnerTcpServer::~InnerTcpServer()
{
}

void InnerTcpServer::incomingConnection(qintptr handle)
{
	innerTcpSocket = new InnerTcpSocket(handle, this);
}

