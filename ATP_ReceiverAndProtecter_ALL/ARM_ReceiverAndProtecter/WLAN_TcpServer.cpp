// mytcpserver.cpp

#include "WLAN_TcpServer.h"
#include "wlan_tcpsocket.h"
#include "QThread"

WLAN_TcpServer::WLAN_TcpServer(QObject *parent)
{
    this->setParent(parent);
    assert(this->listen(QHostAddress::Any, 12300));
    qDebug()<<"WLAN Server is listening on 12300 port of all address.";
}

void WLAN_TcpServer::incomingConnection(qintptr socketDescriptor)
{
    new WLAN_TcpSocket(socketDescriptor,this);
}
