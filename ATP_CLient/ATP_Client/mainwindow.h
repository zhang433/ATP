#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif
#pragma once
//#define ARM_IP "10.42.0.1"
#define ARM_IP "127.0.0.1"
#include <QtGlobal>
#include <QMainWindow>
#include <vector>
#include <QTreeWidget>
#include "basestructer.h"
#include <QTimer>
#include <QDateTime>
#include <QChart>
#include <QLineSeries>
#include <QAreaSeries>
#include <qqueue.h>
#include <QDateTimeAxis>
#include <QValueAxis>
#include <QScatterSeries>
#include <QList>
#include <fstream>
#include <analyzeresultshow_dialog.h>
#include "Command.h"
#include "chartview.h"
#include "callout.h"
#include "getreport_dialog.h"


enum STATUS_BAR {
	COMMAND_SATUS=0,
	DATA_STATUS,
	TRAIN_SEQUENCE,
	BETTERY_STATUS,
	SD_CAPACITY,
};

using namespace std;
QT_CHARTS_USE_NAMESPACE

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
    QLabel *ConnectStateLabel_Command;//用于显示底部状态栏连接信息-命令连接
	QLabel* ConnectStateLabel_Data;//用于显示底部状态栏连接信息-数据连接
    QLabel *SequenceLabel;//用于显示底部状态栏连接信息
	QLabel *BetteryLabel;//用于显示板卡电量信息
	QLabel *SDCardLabel;//用于显示板卡电量信息
	static QMutex m_static_mutex;
	static QQueue<RealTimeDatastructure> RTD_Queue;
	static QQueue<QVector<QVector<QVector<QString>>>>  ResultVec_Queue;
	static QVector<QString> AnalysisDMS_RTD2String(const RealTimeDatastructure& RTD);//分析DMS数据的信息
	void AnalysisInfoClick_SLOT(qint64);//当视图中的重要信息被点击时用于展示点击信息
    explicit MainWindow(QString user,QWidget *parent = 0);
	~MainWindow();
signals:

public slots:
	void UpdateStatusState_SLOT(QString msg, QString backgroud_color, enum STATUS_BAR status_bar);
	void ReDraw_MainWindow();
    void UpdateView(RealTimeDatastructure& RTD);
    void UpdateBaliseCompareResult(RealTimeDatastructure& RTD);//各栏添加比对记录
	void UpdateTabWidget_udp(const QVector<QVector<QString> > &result, CompareType compareType);
	void UpdateTabWidget_click(const QVector<QVector<QString> > &result, CompareType compareType);

private slots:

	void on_Errmsg_LISTWIDGET_clicked(const QModelIndex &index);

	void on_listWidget_trackCircuit_compare_history_clicked(const QModelIndex &index);

	void on_listWidget_neutralSection_compare_history_clicked(const QModelIndex &index);

	void on_listWidget_baliseLocation_compare_history_clicked(const QModelIndex &index);

	void on_listWidget_speed_compare_history_clicked(const QModelIndex &index);

	void on_listWidget_grade_compare_history_clicked(const QModelIndex &index);

	void on_listWidget_station_compare_history_clicked(const QModelIndex &index);

	void on_listWidget_temporySpeed_history_clicked(const QModelIndex &index);

	void on_Config_arg_triggered();

    void on_toolButton_clicked();

    void on_GetRepotr_MENU_triggered();

    void on_SetTrainNumber_MENU_triggered();

	void on_ClearLocal_triggered();//清空本机数据

    void on_ImportATPFiles_MENU_triggered();

    void on_ConfigUserInfo_MENU_triggered();

private:
    static const QString DMS_DONGZUOMOSHI_LIST[20];
	static const QString DMS_ZAIPIN_LIST[10];
	static const QString _200C_ZAIPIN_LIST[19];
	static const QString DMS_DIPIN_LIST[21];
	static const QString _200C_DIPIN_LIST[34];
	static const char mask_novalue[8];
    QVector<QVector<QVector<QString>>> resultVec;
	QVector<BaliseBitMessage> BaliseBit_Vector;
    QVector<QDataStream> RBCBit_Vector;
    BaliseBitMessage * show_BalishData_ptr = nullptr;
    QByteArray * show_RBCData_ptr = nullptr;
	Ui::MainWindow *ui;
    void Change_AnalysisResult(BaliseBitMessage* BBM,QByteArray* RBC);
    AnalyzeResultShow_DIALOG* RBCS_DIALOG = new AnalyzeResultShow_DIALOG();
	RealTimeDatastructure RTD_Static;

    QString UserName;
	QVector<int> m_errorHistoryVec;

    //比对结果存储表
    QVector<QVector<QVector<QString> > > m_balise_location_list;
    QVector<QVector<QVector<QString> > > m_track_circuit_list;
    QVector<QVector<QVector<QString> > > m_speed_list;
    QVector<QVector<QVector<QString> > > m_grade_list;
    QVector<QVector<QVector<QString> > > m_track_station_list;
    QVector<QVector<QVector<QString> > > m_neutral_section_list;
    QVector<QVector<QVector<QString> > > m_temporary_speed_list;

    //错误结果存储表
    QVector<QVector<QVector<QString> > > m_e_balise_location_list;
    QVector<QVector<QVector<QString> > > m_e_track_circuit_list;
    QVector<QVector<QVector<QString> > > m_e_speed_list;
    QVector<QVector<QVector<QString> > > m_e_grade_list;
    QVector<QVector<QVector<QString> > > m_e_track_station_list;
    QVector<QVector<QVector<QString> > > m_e_neutral_section_list;
    QVector<QVector<QVector<QString> > > m_e_temporary_speed_list;
};
