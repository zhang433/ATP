#ifndef __CHARTVIEW_H__
#define __CHARTVIEW_H__
#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif
#include <QChartView>
#include <QRubberBand>
#include <QDateTimeAxis>
#include <QValueAxis>
#include <QDateTime>
#include <qtooltip.h>
#include <qtimer.h>
#include <qlist.h>
#include <qmutex.h>
#include <qstring.h>
#include <qdir.h>
#include <qdebug.h>
#include <algorithm>
#include <QLineSeries>
#include <analyze_rbcmessage.h>
#include "self_define.h"
#include <analyze_resmessage.h>
#include "QtCharts/qscatterseries.h"
#include "Command.h"
#include "callout.h"
#include "qapplication.h"
#include "mainwindow.h"

QT_CHARTS_USE_NAMESPACE
#define MAX_NUM_CALLOUT 30
#define SHOW_INTERVAL 1000*30

class ChartView : public QChartView
{
    Q_OBJECT
public:
	/************用户调用接口**********************/
    ChartView(QWidget *parent);//构造函数
    void UpdateView(const RealTimeDatastructure& RTD,bool move_frame);//示图更新函数
	static void getDataFromFile(qint64 pos,RealTimeDatastructure* RTD);//从指定位置读取RTD变量
	static QMutex m_mutex;//读取文件改变文件指针位置时用于保护临界区，防止多线程同时读写
	void clear();//清理视图中的图标和画线信息
    void moveCenturalToPoint(qreal x);//把视图移到以pos为中心的位置
signals:
	void AnalysisInfoClick_SIGNAL(qint64);
private:
	/****************数据的临时文件*************************/
	RealTimeDatastructure temp_RTD;
	static QMap<qint64, qint64> fp_map;//存储x轴位置到文件存储位置的映射
	static QFile RTDFile;//存储的实际文件对象
	static QDataStream RTDDataStream;//数据以数据流的方式写入到文件中
	/**********用户的界面交互事件处理********************/
    bool isLeftClicking, isRightClicking;//在鼠标拖拽事件中，只有鼠标左键按下才会进行图像移动，此标记用于指示鼠标左键是否按下
	qreal y_min, y_max;//用于限定y轴的显示区间
    int xOld;//用于拖动图像时平移x轴的图像记录值
	void enterEvent(QEvent *) override;//鼠标移入事件
	void leaveEvent(QEvent *) override;//鼠标移出事件
	void mousePressEvent(QMouseEvent *event) override;//鼠标按下事件，左键用于拖拽信号，右键恢复实时模式
	void mouseMoveEvent(QMouseEvent *event) override;//拖拽事件，移动图像
	void mouseReleaseEvent(QMouseEvent *event) override;//鼠标松放事件，清空按压信号
	void wheelEvent(QWheelEvent *event) override;//滚轮事件，用于处理图像缩放的问题
	void mouseDoubleClickEvent(QMouseEvent *event) override;//鼠标双击事件，用于取消图上所有的Callout标定
	/*******************用户与某条曲线间的交互************************/
	Callout* tempTip;//用于鼠标悬停时出现的callout和悬停事件发出的曲线,注意声明顺序要在l_Callout_hide之前
	QVector<Callout*> l_Callout_show,l_Callout_hide;//用于显示定在视图上的callout，通过用户点击曲线触发
	void speedPointTipSlot(const QPointF &point, bool state);//速度曲线的悬停交互
	void baliseTipSlot(const QPointF &point, bool state);//应答器点线的悬停交互
	void rbcTipSlot(const QPointF &point, bool state);//rbc点线的悬停交互
	void freTipSlot(const QPointF &point, bool state);//载频曲线的悬停交互
	void changeAnotherCallout(Callout*& tempTip);//从资源池里置换一个未使用的Callout
	bool getDataByTimeFromFile(qint64 Time, RealTimeDatastructure* RTD);//用于获取某一个标记点的记录信息
	/*******************画图相关*********************/
	qreal y_Fre = -10;//画载频的位置
	qreal y_RBC = -20;//画RBC的位置
	qreal y_Balish = -40;//画应答器的位置
	qint64 nowTimeStamp = 0;//最新更新的坐标点的横坐标
	int freSeriersIndex;
	QLineSeries markVerticalLine, markHorizontalLine;
	QVector<QPointF> markVerticalVec, markHorizontalVec;
	QTimer viewUpdateTimer;
	QVector<QVector<QPointF>> speedVec, balishVec, freVec;
	QVector<QPointF> rbcVec;
	QVector<QColor> freColorVec;
	qreal x_left_point, x_right_point;
	QBrush baliseBrushVec[4];//应答器的画刷要定制
	QVector<QPair<QColor,QString>> mapColor2Fre;//颜色到载频信息的映射
	void initBaliseBrushVec();//初始化应答器画刷
	QDateTimeAxis mAxisX;//横坐标使用时间
	QValueAxis mAxisY;//统一坐标系
	qreal finalXPosition;//图像更新的最后一个位置
	//QLineSeries *markLineSerierVec[2]; //两标记线数据序列
	QLineSeries speedSeriesVec[4];//0-当前速度，1-EBP，2-NBP，3-目标速度，速度直线
	QScatterSeries balishSeriersVec[4];//0-普通应答器，1-有源应答器，2-侧线普通应答器，3-侧线有源应答器，应答器点线
	QScatterSeries rbcSeriersVec;//RBC点线
	QList<QLineSeries*> v_freSeriersVec;//载频直线，该直线是运行时动态添加的
	QPen FrePen;//载频的颜色要运行时变化
	void UpdateSpeed(const RealTimeDatastructure&);//用于更新4个速度最新位置序列
	void UpdateBalishRecord(const RealTimeDatastructure&);//用于更新3个应答器最新位置序列
	void UpdateRBCRecord(const RealTimeDatastructure&);//用于更新RBC最新位置序列
	void UpdateFreRecord(const RealTimeDatastructure&);//用于更新载频最新位置序列
    void updateAllLine(bool move_frame);//更新视图中的所有曲线，如果时间戳在显示区间内就直接更新，否则就忽略
	

	double m_static_lastspeed[4];
	qint8 m_static_last_Fre;
signals:
    void leftMarkClick(QPointF point);
    void rightMarkClick(QPointF point);
private slots:
	void updateAllLine_cyclicity();//周期性的更新视图
	void XrangeChangedSLOT(QDateTime min, QDateTime max);
};

#endif /* __CHARTVIEW_H__ */
