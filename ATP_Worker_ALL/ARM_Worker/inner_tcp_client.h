#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif
#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include <QHostAddress>
#include <QDataStream>
#include <QDebug>
#include <Command.h>
#include "TcpAbstract.h"

class InnerTcpClient : public TcpAbstract
{
    Q_OBJECT
public:
    InnerTcpClient();
signals:
    void send(QByteArray QBA);
    void UdpDataCome_SIGNAL(qint32, qint64);
    void DMSDataCome_SIGNAL(QByteArray, quint8);
private slots:
	void HeartbeatTimeout_SLOT() override;
    void decodeBuffer(QDataStream& ds);
};

extern InnerTcpClient* G_innerTcpClient;
