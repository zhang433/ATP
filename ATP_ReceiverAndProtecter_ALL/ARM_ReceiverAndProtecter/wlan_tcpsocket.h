#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif
#pragma once
#include "Command.h"
#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QList>
#include <QDebug>
#include <QTimer>
#include <QTime>
#include <qbytearray.h>
#include "WLAN_TcpServer.h"
#include "QFile"
#include <QDataStream>
#include <QProcess>
#include "designdata.h"
#include "TcpAbstract.h"
#include "qcoreapplication.h"

class WLAN_TcpSocket:public TcpAbstract
{
    Q_OBJECT
public:
    WLAN_TcpSocket(qintptr socketDescriptor, WLAN_TcpServer*);
    ~WLAN_TcpSocket() override;

protected:
    void decodeBuffer(QDataStream& QDS) override;
protected slots:
    void HeartbeatTimeout_SLOT() override;

private slots:
    void SendBetteryPersent_SLOT();
private:
	void inline ProcessingCommand_DATASHEET(QDataStream& in);
	void inline ProcessingCommand_LIST_FILE(QDataStream& in);
	void inline ProcessingCommand_GET_SELECTED_FILE(QDataStream& in);
	void inline ProcessingCommand_SOCKET_TYPE(QDataStream& in);
    QTimer statusTimer;
};
