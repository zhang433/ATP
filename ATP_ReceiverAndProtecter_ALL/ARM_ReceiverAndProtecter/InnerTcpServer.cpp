#include "InnerTcpServer.h"
#include "QProcess"
InnerTcpServer::InnerTcpServer(QObject* parent):
	innerTcpSocket(nullptr)
{
	this->setParent(parent);
    this->listen(QHostAddress("127.0.0.1"),12345);
    qDebug()<<"InnerTcpServer is listening on 127.0.0.1:12345";
#ifdef _WIN32
    QProcess::startDetached("ARM_Worker.exe");
#endif
#ifdef __linux
    QProcess::startDetached("/home/root/ARM_Worker");
#endif
}

InnerTcpServer::~InnerTcpServer()
{
}

void InnerTcpServer::incomingConnection(qintptr handle)
{
	innerTcpSocket = new InnerTcpSocket(handle, this);
}

