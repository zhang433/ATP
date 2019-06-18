#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif
#pragma once
#include <QTcpSocket>
#include <QThread>
#include "qhostaddress.h" 
#include "mainwindow.h"
#include "qdebug.h"
#include <iostream>
#include "Command.h"
#include <QDataStream>
#include <QApplication>
#include <qmutex.h>
#include <TcpAbstract.h>

using namespace std;

class TcpDataClient:public TcpAbstract
{
	Q_OBJECT
public:
    TcpDataClient();
    ~TcpDataClient();
signals:
	void ReDraw_MainWindow_SIGNAL();
	void UpdateMainWondowStatue_SIGNAL(QString msg, QString backgroud_color, enum STATUS_BAR);
    void init();
private:
    TcpHead TH;
	RealTimeDatastructure RTD;
	void decodeBuffer(QDataStream& QDS);
private slots:
	void changeStatus_SLOT(QAbstractSocket::SocketState state);
    void startConnect();
    void HeartbeatTimeout_SLOT() override;
};
extern TcpDataClient* tcpDataClient;


