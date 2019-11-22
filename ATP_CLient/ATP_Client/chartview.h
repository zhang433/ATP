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
	/************�û����ýӿ�**********************/
    ChartView(QWidget *parent);//���캯��
    void UpdateView(const RealTimeDatastructure& RTD,bool move_frame);//ʾͼ���º���
	static void getDataFromFile(qint64 pos,RealTimeDatastructure* RTD);//��ָ��λ�ö�ȡRTD����
	static QMutex m_mutex;//��ȡ�ļ��ı��ļ�ָ��λ��ʱ���ڱ����ٽ�������ֹ���߳�ͬʱ��д
	void clear();//������ͼ�е�ͼ��ͻ�����Ϣ
    void moveCenturalToPoint(qreal x);//����ͼ�Ƶ���posΪ���ĵ�λ��
signals:
	void AnalysisInfoClick_SIGNAL(qint64);
private:
	/****************���ݵ���ʱ�ļ�*************************/
	RealTimeDatastructure temp_RTD;
	static QMap<qint64, qint64> fp_map;//�洢x��λ�õ��ļ��洢λ�õ�ӳ��
	static QFile RTDFile;//�洢��ʵ���ļ�����
	static QDataStream RTDDataStream;//�������������ķ�ʽд�뵽�ļ���
	/**********�û��Ľ��潻���¼�����********************/
    bool isLeftClicking, isRightClicking;//�������ק�¼��У�ֻ�����������²Ż����ͼ���ƶ����˱������ָʾ�������Ƿ���
	qreal y_min, y_max;//�����޶�y�����ʾ����
    int xOld;//�����϶�ͼ��ʱƽ��x���ͼ���¼ֵ
	void enterEvent(QEvent *) override;//��������¼�
	void leaveEvent(QEvent *) override;//����Ƴ��¼�
	void mousePressEvent(QMouseEvent *event) override;//��갴���¼������������ק�źţ��Ҽ��ָ�ʵʱģʽ
	void mouseMoveEvent(QMouseEvent *event) override;//��ק�¼����ƶ�ͼ��
	void mouseReleaseEvent(QMouseEvent *event) override;//����ɷ��¼�����հ�ѹ�ź�
	void wheelEvent(QWheelEvent *event) override;//�����¼������ڴ���ͼ�����ŵ�����
	void mouseDoubleClickEvent(QMouseEvent *event) override;//���˫���¼�������ȡ��ͼ�����е�Callout�궨
	/*******************�û���ĳ�����߼�Ľ���************************/
	Callout* tempTip;//���������ͣʱ���ֵ�callout����ͣ�¼�����������,ע������˳��Ҫ��l_Callout_hide֮ǰ
	QVector<Callout*> l_Callout_show,l_Callout_hide;//������ʾ������ͼ�ϵ�callout��ͨ���û�������ߴ���
	void speedPointTipSlot(const QPointF &point, bool state);//�ٶ����ߵ���ͣ����
	void baliseTipSlot(const QPointF &point, bool state);//Ӧ�������ߵ���ͣ����
	void rbcTipSlot(const QPointF &point, bool state);//rbc���ߵ���ͣ����
	void freTipSlot(const QPointF &point, bool state);//��Ƶ���ߵ���ͣ����
	void changeAnotherCallout(Callout*& tempTip);//����Դ�����û�һ��δʹ�õ�Callout
	bool getDataByTimeFromFile(qint64 Time, RealTimeDatastructure* RTD);//���ڻ�ȡĳһ����ǵ�ļ�¼��Ϣ
	/*******************��ͼ���*********************/
	qreal y_Fre = -10;//����Ƶ��λ��
	qreal y_RBC = -20;//��RBC��λ��
	qreal y_Balish = -40;//��Ӧ������λ��
	qint64 nowTimeStamp = 0;//���¸��µ������ĺ�����
	int freSeriersIndex;
	QLineSeries markVerticalLine, markHorizontalLine;
	QVector<QPointF> markVerticalVec, markHorizontalVec;
	QTimer viewUpdateTimer;
	QVector<QVector<QPointF>> speedVec, balishVec, freVec;
	QVector<QPointF> rbcVec;
	QVector<QColor> freColorVec;
	qreal x_left_point, x_right_point;
	QBrush baliseBrushVec[4];//Ӧ�����Ļ�ˢҪ����
	QVector<QPair<QColor,QString>> mapColor2Fre;//��ɫ����Ƶ��Ϣ��ӳ��
	void initBaliseBrushVec();//��ʼ��Ӧ������ˢ
	QDateTimeAxis mAxisX;//������ʹ��ʱ��
	QValueAxis mAxisY;//ͳһ����ϵ
	qreal finalXPosition;//ͼ����µ����һ��λ��
	//QLineSeries *markLineSerierVec[2]; //���������������
	QLineSeries speedSeriesVec[4];//0-��ǰ�ٶȣ�1-EBP��2-NBP��3-Ŀ���ٶȣ��ٶ�ֱ��
	QScatterSeries balishSeriersVec[4];//0-��ͨӦ������1-��ԴӦ������2-������ͨӦ������3-������ԴӦ������Ӧ��������
	QScatterSeries rbcSeriersVec;//RBC����
	QList<QLineSeries*> v_freSeriersVec;//��Ƶֱ�ߣ���ֱ��������ʱ��̬��ӵ�
	QPen FrePen;//��Ƶ����ɫҪ����ʱ�仯
	void UpdateSpeed(const RealTimeDatastructure&);//���ڸ���4���ٶ�����λ������
	void UpdateBalishRecord(const RealTimeDatastructure&);//���ڸ���3��Ӧ��������λ������
	void UpdateRBCRecord(const RealTimeDatastructure&);//���ڸ���RBC����λ������
	void UpdateFreRecord(const RealTimeDatastructure&);//���ڸ�����Ƶ����λ������
    void updateAllLine(bool move_frame);//������ͼ�е��������ߣ����ʱ�������ʾ�����ھ�ֱ�Ӹ��£�����ͺ���
	

	double m_static_lastspeed[4];
	qint8 m_static_last_Fre;
signals:
    void leftMarkClick(QPointF point);
    void rightMarkClick(QPointF point);
private slots:
	void updateAllLine_cyclicity();//�����Եĸ�����ͼ
	void XrangeChangedSLOT(QDateTime min, QDateTime max);
};

#endif /* __CHARTVIEW_H__ */
