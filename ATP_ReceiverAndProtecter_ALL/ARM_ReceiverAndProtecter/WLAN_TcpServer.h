#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QList>
#include <QDebug>
#include <QThread>
#include <QTimer>
#include <QDataStream>


class WLAN_TcpServer : public QTcpServer
{
	Q_OBJECT
public:
    explicit WLAN_TcpServer(QObject *parent = nullptr);
signals:
    void sendToAll(QByteArray);
protected:
    void incomingConnection(qintptr socketDescriptor) override;
};

