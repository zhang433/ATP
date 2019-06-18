#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif
#pragma once
#include "TcpAbstract.h"
#include "Command.h"
#include "mainwindow.h"

class TcpCommandClient:public TcpAbstract
{
    Q_OBJECT
public:
    TcpCommandClient();
    ~TcpCommandClient() override;
signals:
    void UpdateMainWondowStatue_SIGNAL(QString msg, QString backgroud_color,enum STATUS_BAR status);
    void send(QByteArray);
    void HasReturn2Sheet_SIGNAL(QString);
    void GetFileList_SIGNAL(QVector<QVector<QString>>);
    void GetSelectedFile_SIGNAL(QVector<SequenceDataStructure> V_SDS);
	void GetDownloadFile_SIGNAL(QVector<QString>, QVector<QByteArray>);
    void changeDialogStatue_SIGNAL(QString, QString);
    void ReDraw_MainWindow_SIGNAL();
    void init();
private:
    TcpHead TH;
    RealTimeDatastructure RTD;
    void decodeBuffer(QDataStream& QDS) override;
    void inline ProcessingCommand_DATA_SHEET(QDataStream& ds);
    void inline ProcessingCommand_CHANGE_ARGUMENT(QDataStream& ds);
    void inline ProcessingCommand_LIST_FILE(QDataStream& ds);
    void inline ProcessingCommand_GET_SELECTED_FILE(QDataStream& ds);
private slots:
	void changeStatus_SLOT(QAbstractSocket::SocketState state);
    void startConnect();
    void HeartbeatTimeout_SLOT() override;
};
extern TcpCommandClient* tcpCommandClient;
