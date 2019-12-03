#include "chartview.h"
//#define USEOPENGL//定义该宏表示视图界面的线使用GPU渲染，在MinGW编译时会出错
//初始化静态对象成员
QMap<qint64, qint64> ChartView::fp_map;
QMutex ChartView::m_mutex;
QFile ChartView::RTDFile;
QDataStream ChartView::RTDDataStream;

ChartView::ChartView(QWidget *parent) :
	QChartView(parent),
	isLeftClicking(false), isRightClicking(false),
	y_min(-50), y_max(350),
	xOld(0), finalXPosition(0),
	tempTip(new Callout(chart())),
	l_Callout_hide({ tempTip }),
	x_left_point(0), x_right_point(1000),
	speedVec{ QVector<QPointF>({QPointF(0.0,0.0),QPointF(0.0,0.0)}),QVector<QPointF>({QPointF(0.0,0.0),QPointF(0.0,0.0)}),QVector<QPointF>({QPointF(0.0,0.0),QPointF(0.0,0.0)}),QVector<QPointF>({QPointF(0.0,0.0),QPointF(0.0,0.0)}) },
	balishVec{ QVector<QPointF>(),QVector<QPointF>(),QVector<QPointF>(),QVector<QPointF>() }, freSeriersIndex{ 0 },
	markVerticalVec{ QPointF(0.0,y_min), QPointF(0.0,y_max) },
	markHorizontalVec{ QPointF(0.0,0.0), QPointF(0.0,0.0) },
	m_static_lastspeed{ 0.0,0.0,0.0,0.0 }, m_static_last_Fre{-1},
	mapColor2Fre({ QPair<QColor,QString>(QColor(0,0,0),"Unknown"),\
		QPair<QColor, QString>(QColor(220, 20, 60), "0"),\
		QPair<QColor, QString>(QColor(139, 0, 139), "550"),\
		QPair<QColor, QString>(QColor(148, 0, 211), "650"),\
		QPair<QColor, QString>(QColor(0, 0, 255), "750"),\
		QPair<QColor, QString>(QColor(176, 196, 222), "850"),\
		QPair<QColor, QString>(QColor(0, 191, 255), "1700"),\
		QPair<QColor, QString>(QColor(95, 158, 160), "2000"),\
		QPair<QColor, QString>(QColor(0, 255, 0), "2300"),\
		QPair<QColor, QString>(QColor(255, 215, 0), "2600"),\
		QPair<QColor, QString>(QColor(255, 255, 255), "unexpected value") })
{
	QColor c(0, 0, 0xcd);
	QBrush B(Qt::DashLine);
	B.setColor(c);
	
	markVerticalLine.setPen(QPen(Qt::SolidPattern));
	markVerticalLine.setColor(QColor(0, 0, 0xcd));
	//markVerticalLine.setBrush(B);
	markHorizontalLine.setPen(QPen(Qt::DashDotLine));
	markHorizontalLine.setColor(QColor(0, 0, 0xcd));
	//markHorizontalLine.setBrush(B);
	FrePen.setWidth(15);
	//初始化应答器所需颜色和形状
	mAxisX.setRange(QDateTime::fromMSecsSinceEpoch(QDateTime::currentMSecsSinceEpoch()), QDateTime::fromMSecsSinceEpoch(QDateTime::currentMSecsSinceEpoch()+1000*30));
	markHorizontalVec[0].setX(mAxisX.min().toMSecsSinceEpoch());
	markHorizontalVec[1].setX(mAxisX.max().toMSecsSinceEpoch());

	mAxisX.setFormat("hh:mm:ss");
	//mAxisX.setFormat("hh:mm:ss");
	mAxisX.setTickCount(5);

	mAxisY.setRange(y_min, y_max);
	mAxisY.setTickCount(9);

	chart()->addAxis(&mAxisX, Qt::AlignBottom);
	chart()->addAxis(&mAxisY, Qt::AlignLeft);

	chart()->axisX()->setGridLineVisible(true);
	chart()->axisY()->setGridLineVisible(true);

	//添加线路速度的绘图曲线
	QPen pen;
	pen.setWidthF(2.5);
	pen.setColor(QColor(0,0,0));
	speedSeriesVec[0].setPen(pen);
	pen.setColor(QColor(255,0,0));
	speedSeriesVec[1].setPen(pen);
	pen.setColor(QColor(128,128,0));
	speedSeriesVec[2].setPen(pen);
	pen.setColor(QColor(0,170,255));
	speedSeriesVec[3].setPen(pen);
	for (int i = 0; i < sizeof(speedSeriesVec) / sizeof(speedSeriesVec[0]); i++)
	{
        //speedSeriesVec[i].setUseOpenGL();
		chart()->addSeries(&speedSeriesVec[i]);
		speedSeriesVec[i].attachAxis(&mAxisX); //关联坐标轴
		speedSeriesVec[i].attachAxis(&mAxisY);
        //connect(&speedSeriesVec[i], &QLineSeries::hovered, this, &ChartView::speedPointTipSlot);
	}
	//为画应答器的线定制图形
	initBaliseBrushVec();
	for (int i = 0; i < 4; i++)
	{
        //balishSeriersVec[i].setUseOpenGL();
		balishSeriersVec[i].setBrush(baliseBrushVec[i]);
		balishSeriersVec[i].setPen(QColor(Qt::transparent));
		balishSeriersVec[i].setMarkerShape(QScatterSeries::MarkerShapeRectangle);
		balishSeriersVec[i].setMarkerSize(20.0);

		chart()->addSeries(&balishSeriersVec[i]);
		balishSeriersVec[i].attachAxis(&mAxisX); //关联坐标轴
		balishSeriersVec[i].attachAxis(&mAxisY);
		connect(&balishSeriersVec[i], &QScatterSeries::hovered, this, &ChartView::baliseTipSlot);
	}
	//RBC线的添加到绘图界面
	//rbcSeriersVec.setUseOpenGL();
	rbcSeriersVec.setMarkerShape(QScatterSeries::MarkerShapeRectangle);
	rbcSeriersVec.setMarkerSize(15.0);
	chart()->addSeries(&rbcSeriersVec);
	rbcSeriersVec.attachAxis(&mAxisX); //关联坐标轴
	rbcSeriersVec.attachAxis(&mAxisY);
	connect(&rbcSeriersVec, &QScatterSeries::hovered, this, &ChartView::rbcTipSlot);

	chart()->legend()->hide(); //隐藏图例
	setRenderHint(QPainter::Antialiasing);

	//markVerticalLine.setUseOpenGL();
	//markHorizontalLine.setUseOpenGL();
	chart()->addSeries(&markHorizontalLine);
	markHorizontalLine.attachAxis(&mAxisX); //关联坐标轴
	markHorizontalLine.attachAxis(&mAxisY);
	chart()->addSeries(&markVerticalLine);
	markVerticalLine.attachAxis(&mAxisX); //关联坐标轴
	markVerticalLine.attachAxis(&mAxisY);

	//初始化文件
	RTDFile.setFileName(QString("%1/rtdflie.bin").arg(QDir::currentPath()));
	if (!RTDFile.open(QIODevice::ReadWrite))
	{
		QMessageBox::StandardButton rb = QMessageBox::information(NULL, "提示", "存储文件创建失败，退出程序");
		exit(-1);
	}
	else
	{
		RTDDataStream.setDevice(&RTDFile);
	}
	//viewUpdateTimer.setInterval(1000);
	//connect(&viewUpdateTimer, SIGNAL(timeout()), this, SLOT(updateAllLine_cyclicity()));
	//viewUpdateTimer.start(1000);//为了CPU渲染做的优化，每隔一段时间同一刷新视图，否则会有明显卡顿
	//X轴坐标范围变动时的信号和槽
	connect(&mAxisX, &QDateTimeAxis::rangeChanged, this, &ChartView::XrangeChangedSLOT);
}

void ChartView::clear()
{
	finalXPosition = 0;
	//清空画图中的曲线
	for(int i=0;i<sizeof(speedSeriesVec)/sizeof(speedSeriesVec[0]);++i)
		chart()->removeSeries(&speedSeriesVec[i]);
	for (int i = 0; i < sizeof(balishSeriersVec) / sizeof(balishSeriersVec[0]); ++i)
		chart()->removeSeries(&balishSeriersVec[i]);
	chart()->removeSeries(&rbcSeriersVec);
	for(auto& s: v_freSeriersVec)
		chart()->removeSeries(s);
	v_freSeriersVec.clear();
	//清空曲线中的数据点
	rbcSeriersVec.clear();
	for (int i = 0; i < sizeof(balishSeriersVec) / sizeof(balishSeriersVec[0]); ++i)
		balishSeriersVec[i].clear();
	for (int i = 0; i < sizeof(speedSeriesVec) / sizeof(speedSeriesVec[0]); i++)
		speedSeriesVec[i].clear();

	//清空记录的数据点
	fp_map.clear();
	speedVec = { QVector<QPointF>({QPointF(0.0,0.0),QPointF(0.0,0.0)}),QVector<QPointF>({QPointF(0.0,0.0),QPointF(0.0,0.0)}),QVector<QPointF>({QPointF(0.0,0.0),QPointF(0.0,0.0)}),QVector<QPointF>({QPointF(0.0,0.0),QPointF(0.0,0.0)}) };
	balishVec = { QVector<QPointF>(),QVector<QPointF>(),QVector<QPointF>(),QVector<QPointF>() };
	m_static_last_Fre = -1;
	freSeriersIndex = 0;
	freColorVec.clear();
	freVec.clear();

	for (int i = 0; i < sizeof(m_static_lastspeed) / sizeof(m_static_lastspeed[0]); ++i)
		m_static_lastspeed[i] = 0.0;
	for (int i = 0; i < sizeof(speedSeriesVec) / sizeof(speedSeriesVec[0]); i++)
	{
#ifdef USEOPENGL
		speedSeriesVec[i].setUseOpenGL();
#endif
		chart()->addSeries(&speedSeriesVec[i]);
		speedSeriesVec[i].attachAxis(&mAxisX); //关联坐标轴
		speedSeriesVec[i].attachAxis(&mAxisY);
		connect(&speedSeriesVec[i], &QLineSeries::hovered, this, &ChartView::speedPointTipSlot);
	}
	//为画应答器的线定制图形
	initBaliseBrushVec();
	for (int i = 0; i < 4; i++)
	{
		//balishSeriersVec[i].setUseOpenGL();
		balishSeriersVec[i].setBrush(baliseBrushVec[i]);
		balishSeriersVec[i].setPen(QColor(Qt::transparent));
		balishSeriersVec[i].setMarkerShape(QScatterSeries::MarkerShapeRectangle);
		balishSeriersVec[i].setMarkerSize(20.0);

		chart()->addSeries(&balishSeriersVec[i]);
		balishSeriersVec[i].attachAxis(&mAxisX); //关联坐标轴
		balishSeriersVec[i].attachAxis(&mAxisY);
		connect(&balishSeriersVec[i], &QScatterSeries::hovered, this, &ChartView::baliseTipSlot);
	}
	//RBC线的添加到绘图界面
	//rbcSeriersVec.setUseOpenGL();
	rbcSeriersVec.setMarkerShape(QScatterSeries::MarkerShapeRectangle);
	rbcSeriersVec.setMarkerSize(15.0);
	chart()->addSeries(&rbcSeriersVec);
	rbcSeriersVec.attachAxis(&mAxisX); //关联坐标轴
	rbcSeriersVec.attachAxis(&mAxisY);
	connect(&rbcSeriersVec, &QScatterSeries::hovered, this, &ChartView::rbcTipSlot);

	chart()->update();
}

void ChartView::getDataFromFile(qint64 x, RealTimeDatastructure* RTD)
{
	QMutexLocker lc(&m_mutex);
	RTDFile.flush();
	Q_ASSERT(fp_map.find(x) != fp_map.end());
	qint64 curent_pos = RTDDataStream.device()->pos();
	RTDDataStream.device()->seek(fp_map[x]);
	RTDDataStream >> (*RTD);
	RTDDataStream.device()->seek(curent_pos);
	return;
}

void ChartView::UpdateView(const RealTimeDatastructure& RTD,bool move_frame)
{
	//写入文件并记录文件写入的位置
	
	if (fp_map.empty())
	{
		chart()->axisX()->setRange(QDateTime::fromMSecsSinceEpoch(RTD.IPCTimestamp), QDateTime::fromMSecsSinceEpoch(RTD.IPCTimestamp +SHOW_INTERVAL));
		for (auto& v : speedVec)
		{
			v[0].setX(RTD.IPCTimestamp - 2);
			v[1].setX(RTD.IPCTimestamp - 1);
		}
	}
	{
		QMutexLocker lc(&m_mutex);
		fp_map[RTD.IPCTimestamp] = RTDDataStream.device()->pos();
		RTDDataStream << RTD;
	}

	//更新记录容器
	UpdateSpeed(RTD);
	UpdateBalishRecord(RTD);
	UpdateRBCRecord(RTD);
	UpdateFreRecord(RTD);
	//更新视图中的曲线
	nowTimeStamp = RTD.IPCTimestamp;
    updateAllLine(move_frame);//CPU渲染时请将此操作设为间隔定时执行
}

void ChartView::UpdateSpeed(const RealTimeDatastructure& RTD)
{
	auto convert_to_double = [&RTD,this](qint16 speed, int index) {
		m_static_lastspeed[index] = MEANINGLESS(speed) ? m_static_lastspeed[index] : speed;
		return m_static_lastspeed[index];
	};
	speedVec[0].push_back(QPointF(RTD.IPCTimestamp, convert_to_double(RTD.MAINT_VIT_TRAIN_LCD, 0)));
	speedVec[1].push_back(QPointF(RTD.IPCTimestamp, convert_to_double(RTD.MAINT_KV_VIT_CTRL_FU, 1)));
	speedVec[2].push_back(QPointF(RTD.IPCTimestamp, convert_to_double(RTD.MAINT_VIT_PRE_INTERV_LCD, 2)));
	speedVec[3].push_back(QPointF(RTD.IPCTimestamp, convert_to_double(RTD.MAINT_VIT_OBJECTIF_LCD, 3)));
}

void ChartView::UpdateBalishRecord(const RealTimeDatastructure& RTD)
{
	if (RTD.Has_Balish_Compare_Result)
	{
		switch (RTD.BalishData.type)
		{
		case BaliseType::Normal:
			balishVec[0].push_back(QPointF(RTD.IPCTimestamp, y_Balish));
			break;
		case BaliseType::Active:
			balishVec[1].push_back(QPointF(RTD.IPCTimestamp, y_Balish));
			break;
		case BaliseType::SideLine | BaliseType::Normal:
			balishVec[2].push_back(QPointF(RTD.IPCTimestamp, y_Balish));
			break;
		case BaliseType::SideLine | BaliseType::Active:
			balishVec[3].push_back(QPointF(RTD.IPCTimestamp, y_Balish));
			break;
		}
	}
}

void ChartView::UpdateRBCRecord(const RealTimeDatastructure &RTD)
{
	if (RTD.Has_DMS_RBC_DATA)
	{
		rbcVec.push_back(QPointF(RTD.IPCTimestamp, y_RBC));
	}
}

void ChartView::UpdateFreRecord(const RealTimeDatastructure &RTD)
{
	if (m_static_last_Fre == -1)//第一次进来
	{
		m_static_last_Fre = 0;
		freColorVec.append(mapColor2Fre[m_static_last_Fre].first);
		freVec.append(QVector<QPointF>{QPointF(RTD.IPCTimestamp, y_Fre), QPointF(RTD.IPCTimestamp, y_Fre)});
	}
	if (MEANINGLESS(RTD.CTCS_CODE_EDIT) || RTD.CTCS_CODE_EDIT == m_static_last_Fre)//数据无法提供或者和上次一样
		freVec.back()[1] = QPointF(RTD.IPCTimestamp, y_Fre);
	else//新的载频
	{
		m_static_last_Fre = RTD.CTCS_CODE_EDIT;
		freColorVec.append(mapColor2Fre[RTD.CTCS_CODE_EDIT].first);

		freVec.back()[1].setX(RTD.IPCTimestamp);//上一个区段的终点延伸到新的坐标点
		freVec.append(QVector<QPointF>{QPointF(RTD.IPCTimestamp, y_Fre), QPointF(RTD.IPCTimestamp, y_Fre)});//添加一个新的区段
	}
}

void ChartView::initBaliseBrushVec()
{
	QPainterPath pathVec[4];

	//有源
	pathVec[0].moveTo(9, 6);
	pathVec[0].lineTo(19, 19);
	pathVec[0].lineTo(0, 19);
	pathVec[0].lineTo(9, 6);
	pathVec[0].closeSubpath();

	//无源
	pathVec[1].moveTo(9, 6);
	pathVec[1].lineTo(19, 19);
	pathVec[1].lineTo(0, 19);
	pathVec[1].lineTo(9, 6);
	pathVec[1].closeSubpath();

	//侧线有源
	pathVec[2].lineTo(9, 6);
	pathVec[2].lineTo(19, 19);
	pathVec[2].lineTo(0, 19);
	pathVec[2].lineTo(9, 6);
	pathVec[2].closeSubpath();

	//侧线无源
	pathVec[3].lineTo(9, 6);
	pathVec[3].lineTo(19, 19);
	pathVec[3].lineTo(0, 19);
	pathVec[3].lineTo(9, 6);
	pathVec[3].closeSubpath();

	for (int i = 0; i < 4; i++)
	{
		QImage image(20, 20, QImage::Format_ARGB32);
		image.fill(Qt::transparent);

		QPainter painter(&image);
		painter.setRenderHint(QPainter::Antialiasing);
		//painter.setPen(baliseColor[i]);
		painter.drawPath(pathVec[i]);
		if (i == 0 || i == 2)
		{
			painter.fillPath(pathVec[i], Qt::darkYellow);
		}

		baliseBrushVec[i] = QBrush(image);
	}
}

/*****************************************用户的界面交互事件处理*************************************************/
void ChartView::wheelEvent(QWheelEvent *event)//滚轮事件，用于处理图像缩放的问题
{
    QPointF centerPoint = chart()->mapToValue(event->posF());
    if (event->delta() > 0) {
		if (mAxisX.max().toMSecsSinceEpoch() - mAxisX.min().toMSecsSinceEpoch() > 500)
		{
			this->chart()->zoom(1.1);
			this->chart()->axisY()->setRange(y_min, y_max);
		}
    } else {
		if (mAxisX.max().toMSecsSinceEpoch() - mAxisX.min().toMSecsSinceEpoch() < 1000*3600)
		{
			this->chart()->zoom(10.0 / 11);
			this->chart()->axisY()->setRange(y_min, y_max);
		}
    }
	emit QChartView::wheelEvent(event);//让事件继续转发
}

void ChartView::mousePressEvent(QMouseEvent *event)//鼠标按下事件，左键用于拖拽信号，右键恢复实时模式
{
	if (event->button() & Qt::LeftButton)//标定框常可见，并设置按压记号，用于拖拽
	{
		changeAnotherCallout(tempTip);
		isLeftClicking = true;
		xOld = event->x();
	}
	else if (event->button() & Qt::RightButton) {//隐藏所有的Callout标定
		isRightClicking = true;
		xOld = event->x();
	}
	emit QChartView::mousePressEvent(event);//让事件继续转发
}

void ChartView::mouseMoveEvent(QMouseEvent *event)//拖拽事件，移动图像
{
	int x, y;
	if (isLeftClicking) {
		//限定x轴的移动范围
		//qreal left_point = mAxisX.min();
		//x = qMin(qreal(event->x() - xOld), left_point-x_left_point);
		x = event->x() - xOld;
		chart()->scroll(-x, 0);
		xOld = event->x();
	}
	else if (isRightClicking)
	{
		QPointF XY_pos(chart()->mapToValue(QPointF(event->x(), event->y()), &markVerticalLine));
		markVerticalVec[0].setX(XY_pos.x());
		markVerticalVec[1].setX(XY_pos.x());
		markVerticalLine.replace(markVerticalVec);

		markHorizontalVec[0].setY(XY_pos.y());
		markHorizontalVec[1].setY(XY_pos.y());
		markHorizontalLine.replace(markHorizontalVec);

		tempTip->hide();
		if (getDataByTimeFromFile(XY_pos.x(), &temp_RTD))
		{
			tempTip->RTD_position = -1;
			QVector<QString> showMsg_set = MainWindow::AnalysisDMS_RTD2String(temp_RTD);
			QString showMsg;
			for (auto& str : showMsg_set)
			{
				showMsg.append(str);
				showMsg.append("\n");
			}
			if (showMsg.endsWith("\n"))
				showMsg.resize(showMsg.size() - 1);
			tempTip->setText(showMsg);
			tempTip->setAnchor(XY_pos);
			tempTip->setZValue(11);
			tempTip->updateGeometry(1, 0);
			tempTip->show();
		}
		this->repaint();
	}
	emit QChartView::mouseMoveEvent(event);
}

void ChartView::mouseReleaseEvent(QMouseEvent *event)//鼠标松放事件，清空按压信号
{
	if (isLeftClicking) {
		isLeftClicking = false;
	}
	else if (isRightClicking)
	{
		isRightClicking = false;
		changeAnotherCallout(tempTip);//置换一个新的Callout
	}
	emit QChartView::mouseReleaseEvent(event);//让事件继续转发
}

void ChartView::moveCenturalToPoint(qreal x)//把视图位置的中心移到最后的点
{
	qreal interval = (mAxisX.max().toMSecsSinceEpoch() - mAxisX.min().toMSecsSinceEpoch())/2;
	mAxisX.setRange(QDateTime::fromMSecsSinceEpoch(x - interval), QDateTime::fromMSecsSinceEpoch(x + interval));
	//qreal interval = (mAxisX.max() - mAxisX.min()) / 2;
	//mAxisX.setRange(x - interval, x + interval);
	//qDebug() << mAxisX.max();
}

void ChartView::mouseDoubleClickEvent(QMouseEvent *event)//恢复实时绘图模式
{
	//chart()->zoomReset();
/*    if (event->button()&Qt::LeftButton)
        moveCenturalToPoint(finalXPosition);
    else */if (event->button()&Qt::RightButton)
    {
        for (auto iter = l_Callout_show.begin(); iter != l_Callout_show.end(); ++iter)
            (*iter)->hide();
        l_Callout_hide.append(l_Callout_show);
        l_Callout_show.clear();
        if (tempTip == nullptr)
            tempTip = l_Callout_hide.front();
    }
}

void ChartView::enterEvent(QEvent *)//鼠标移入事件
{
	//markVerticalLine.show();
	//markHorizontalLine.show();
}

void ChartView::leaveEvent(QEvent *)//鼠标移出事件
{
	//markVerticalLine.hide();
	//markHorizontalLine.hide();
}

/*************************用户与某条曲线间的交互**********************************/
void ChartView::speedPointTipSlot(const QPointF &point, bool state)//速度曲线的悬停
{
	if (tempTip)
	{
		if (state)
		{
			//auto iter = qLowerBound(fp_map->begin(), fp_map->end(), point.x());
			//if (iter != fp_map->end())
			{
				QString Title;
				if (sender() == reinterpret_cast<QObject*>(&speedSeriesVec[0]))
					Title = "运行速度";
				else if (sender() == reinterpret_cast<QObject*>(&speedSeriesVec[1]))
					Title = "EBP速度";
				else if (sender() == reinterpret_cast<QObject*>(&speedSeriesVec[2]))
					Title = "NBP速度";
				else if (sender() == reinterpret_cast<QObject*>(&speedSeriesVec[3]))
					Title = "目标速度";
				tempTip->RTD_position = -1;
				tempTip->setText(QString(Title + "\n时间:%1\n速度:%2KM/h").arg(QDateTime::fromMSecsSinceEpoch(point.x()).toString("hh:mm:ss")).arg(point.y()));
				tempTip->setAnchor(point);
				tempTip->setZValue(11);
				tempTip->updateGeometry(10,5);
				tempTip->show();
			}
		}
		else
		{
			tempTip->hide();
		}
	}
}
void ChartView::baliseTipSlot(const QPointF &point, bool state)//应答器点线的悬停交互
{
	if (tempTip)
	{
		Q_ASSERT(fp_map.find(point.x()) != fp_map.end());
		if (state)
		{
			RealTimeDatastructure RTD;
			getDataFromFile(point.x(), &RTD);
			if (!RTD.Has_Balish_Compare_Result)
			{
				tempTip->hide();
				return;
			}
            //assert(RTD.Has_Balish_Compare_Result);
			QString Title = Analyze_BaliseMessage::GetBaliseID(RTD.BalishData);

			tempTip->RTD_position = point.x();
			tempTip->setZValue(11);
			tempTip->setText(Title + QString("\r\n时间:%1").arg(QDateTime::fromMSecsSinceEpoch(point.x()).toString("hh:mm:ss")));
			tempTip->setAnchor(point);
			tempTip->updateGeometry(10,5);
			tempTip->show();
		}
		else
		{
			tempTip->hide();
		}
	}
}
void ChartView::rbcTipSlot(const QPointF &point, bool state)//rbc点线的悬停交互
{
	if (tempTip)
	{
		Q_ASSERT(fp_map.find(point.x()) != fp_map.end());
		if (state)
		{
			RealTimeDatastructure RTD;
			getDataFromFile(point.x(), &RTD);
			Q_ASSERT(RTD.Has_DMS_RBC_DATA);
			QVector<ItemRBC> v_RBCitem;
			Analyze_RBCMessage::GetMsgID(RTD.RBC, &v_RBCitem);//解析获取RBC消息头

			tempTip->RTD_position = point.x();
			tempTip->setText(v_RBCitem[0].remark + QString("\r\n时间:%1").arg(QDateTime::fromMSecsSinceEpoch(point.x()).toString("hh:mm:ss")));
			tempTip->setAnchor(point);
			tempTip->setZValue(11);
			tempTip->updateGeometry(10,5);
			tempTip->show();
		}
		else
			tempTip->hide();
	}
}
void ChartView::freTipSlot(const QPointF &point, bool state)//载频曲线的悬停交互
{
	if (tempTip)
	{
		if (state)
		{
			QColor c = reinterpret_cast<QLineSeries*>(sender())->pen().color();
			auto iter = std::find_if(mapColor2Fre.begin(), mapColor2Fre.end(), [&c](QPair<QColor, QString>& c_q) {return c_q.first == c; });
			Q_ASSERT(iter != mapColor2Fre.end());
			QString Title = iter->second;

			tempTip->RTD_position = -1;
			tempTip->setText("载频:" + Title + QString("\r\n时间:%1").arg(QDateTime::fromMSecsSinceEpoch(point.x()).toString("hh:mm:ss")));
			tempTip->setAnchor(point);
			tempTip->setZValue(11);
			tempTip->updateGeometry(10,5);
			tempTip->show();
		}
		else
			tempTip->hide();
	}
}

void ChartView::XrangeChangedSLOT(QDateTime min, QDateTime max)
{
	if (tempTip&&tempTip->isVisible())
		tempTip->hide();
	markHorizontalVec[0].setX(mAxisX.min().toMSecsSinceEpoch());
	markHorizontalVec[1].setX(mAxisX.max().toMSecsSinceEpoch());
	markHorizontalLine.replace(markHorizontalVec);
	auto compare = [](const QPointF& p1, const QPointF& p2)->bool {
		return p1.x() < p2.x();
	};
	QVector<QPointF> interval{ QPointF(min.toMSecsSinceEpoch(), 0) ,QPointF(max.toMSecsSinceEpoch(), 0) };
	for (int i = 0; i < 4; ++i)
	{
		auto iter1 = qLowerBound(speedVec[i].begin(), speedVec[i].end(), interval[0], compare);
		if (iter1 != speedVec[i].begin() && (iter1 == speedVec[i].end() || iter1->x() > interval[0].x()))//如果有可能，在容器中找到第一个小于等于区间左端点的记录点
			iter1--;
		auto iter2 = qUpperBound(speedVec[i].begin(), speedVec[i].end(), interval[1], compare);
		speedSeriesVec[i].replace(speedVec[i].mid(iter1 - speedVec[i].begin(), iter2 - iter1 + 1));
	}
	for (int i = 0; i < 4; ++i)
	{
		auto iter1 = qLowerBound(balishVec[i].begin(), balishVec[i].end(), interval[0], compare);
		if (iter1 != balishVec[i].begin() && (iter1 == balishVec[i].end() || iter1->x() > interval[0].x()))//如果有可能，在容器中找到第一个小于等于区间左端点的记录点
			iter1--;
		auto iter2 = qUpperBound(balishVec[i].begin(), balishVec[i].end(), interval[1], compare);
		balishSeriersVec[i].replace(balishVec[i].mid(iter1 - balishVec[i].begin(), iter2 - iter1 + 1));
	}
	auto iter1 = qLowerBound(rbcVec.begin(), rbcVec.end(), interval[0], compare);
	if (iter1 != rbcVec.begin() && (iter1 == rbcVec.end() || iter1->x() > interval[0].x()))//如果有可能，在容器中找到第一个小于等于区间左端点的记录点
		iter1--;
	auto iter2 = qUpperBound(rbcVec.begin(), rbcVec.end(), interval[1], compare);
	rbcSeriersVec.replace(rbcVec.mid(iter1 - rbcVec.begin(), iter2 - iter1 + 1));

	//载频的动态加载处理要复杂很多
	auto iter3 = qLowerBound(freVec.begin(), freVec.end(), interval, [](const QVector<QPointF>& V_P1, const QVector<QPointF>& V_P2)->bool {
		return V_P1[0].x() < V_P2[0].x();
	});
	if (iter3 != freVec.begin() && (iter3 == freVec.end() || (*iter3)[0].x() > interval[0].x()))//如果有可能，在容器中找到第一个小于等于区间左端点的记录点
		iter3--;
	auto iter4 = qUpperBound(freVec.begin(), freVec.end(), interval, [](const QVector<QPointF>& V_P1, const QVector<QPointF>& V_P2)->bool {
		return V_P1[1].x() < V_P2[1].x();
	});
	//[iter3,iter4]是左右双闭区间,
	if (iter4 != freVec.end())
		iter4++;
	freSeriersIndex = 0;
	for (auto iter = iter3; iter != iter4; ++iter)
	{
		if (v_freSeriersVec.size() == freSeriersIndex)//如果载频线的资源池的资源已经耗尽，那么就添加一条
		{
			v_freSeriersVec.push_back(new QLineSeries(this));
			FrePen.setColor(freColorVec[iter - freVec.begin()]);
			v_freSeriersVec.back()->setPen(FrePen);
#ifdef USEOPENGL
            v_freSeriersVec.back()->setUseOpenGL();
#endif
			chart()->addSeries(v_freSeriersVec.back());
			v_freSeriersVec.back()->attachAxis(&mAxisX); //关联坐标轴
			v_freSeriersVec.back()->attachAxis(&mAxisY);
			connect(v_freSeriersVec.back(), &QLineSeries::hovered, this, &ChartView::freTipSlot);
			v_freSeriersVec.back()->replace(*iter);
		}
		else//资源池里还有数据
		{
			FrePen.setColor(freColorVec[iter - freVec.begin()]);
			v_freSeriersVec[freSeriersIndex]->setPen(FrePen);
			v_freSeriersVec[freSeriersIndex]->replace(*iter);
			if (!v_freSeriersVec[freSeriersIndex]->isVisible())
				v_freSeriersVec[freSeriersIndex]->show();
		}
		freSeriersIndex++;
	}
	//如果还有其余载频线在显示状态，就把它们隐藏掉
	while (freSeriersIndex != v_freSeriersVec.size())
	{
		if (v_freSeriersVec[freSeriersIndex]->isVisible())
			v_freSeriersVec[freSeriersIndex]->hide();
		else
			break;
	}
}

void ChartView::updateAllLine_cyclicity()
{
	qDebug() << "update";
	if (finalXPosition <= mAxisX.max().toMSecsSinceEpoch() && nowTimeStamp >= mAxisX.max().toMSecsSinceEpoch())//否则如果是实时模式，即上一个坐标点在区间内，本次坐标点在区间外，就移动区间
	{
		moveCenturalToPoint(nowTimeStamp);//区间变化的槽函数会负责更新视图
	}
	else
	{
		XrangeChangedSLOT(mAxisX.min(), mAxisX.max());
	}
	finalXPosition = nowTimeStamp;//记录上一个更新视图的坐标点
}

void ChartView::updateAllLine(bool move_fram)
{
	auto refreshFreLine = [this]()->void{
		if (freSeriersIndex - 1 >= 0 && freVec.size() - 2 >= 0)
			v_freSeriersVec[freSeriersIndex - 1]->replace(freVec[freVec.size() - 2]);
		if (v_freSeriersVec.size() == freSeriersIndex)//如果载频线的资源池的资源已经耗尽，那么就添加一条
		{
			v_freSeriersVec.push_back(new QLineSeries(this));
			FrePen.setColor(freColorVec.back());
			v_freSeriersVec.back()->setPen(FrePen);
#ifdef USEOPENGL
            v_freSeriersVec.back()->setUseOpenGL();
#endif
			chart()->addSeries(v_freSeriersVec.back());
			v_freSeriersVec.back()->attachAxis(&mAxisX); //关联坐标轴
			v_freSeriersVec.back()->attachAxis(&mAxisY);
			connect(v_freSeriersVec.back(), &QLineSeries::hovered, this, &ChartView::freTipSlot);
			v_freSeriersVec.back()->replace(freVec.back());
		}
		else//资源池里还有数据
		{
			FrePen.setColor(freColorVec.back());
			v_freSeriersVec[freSeriersIndex]->setPen(FrePen);
			v_freSeriersVec[freSeriersIndex]->replace(freVec.back());
			if (!v_freSeriersVec[freSeriersIndex]->isVisible())
				v_freSeriersVec[freSeriersIndex]->show();
		}
		freSeriersIndex++;
	};
	if (nowTimeStamp >= mAxisX.min().toMSecsSinceEpoch() && nowTimeStamp <= mAxisX.max().toMSecsSinceEpoch())//在区间内就直接push_back
	{
		//更新速度,一定有最新的速度
		for (int i = 0; i < 4; ++i)
			speedSeriesVec[i].append(speedVec[i].back());
		//更新RBC,不一定有最新的RBC
		if (!rbcVec.isEmpty() && rbcVec.back().x() == nowTimeStamp)
			rbcSeriersVec.append(rbcVec.back());
		//更新应答器，不一定有最新的应答器
		for (int i = 0; i < 4; ++i)
		{
			if (!balishVec[i].isEmpty() && balishVec[i].back().x() == nowTimeStamp)
				balishSeriersVec[i].append(balishVec[i].back());
		}
		//更新载频，一定有最新的载频，但是不一定在同一条线上
		if (freSeriersIndex == 0)//没有正在显示的载频
		{
			refreshFreLine();
		}
		else
		{
			if (v_freSeriersVec[freSeriersIndex - 1]->pen().color() == freColorVec.back())
				v_freSeriersVec[freSeriersIndex - 1]->append(freVec.back().back());
			else
			{
				refreshFreLine();
			}
		}

	}
    if(move_fram)
    {
        if (finalXPosition <= mAxisX.max().toMSecsSinceEpoch())//否则如果是实时模式，即上一个坐标点在区间内，本次坐标点在区间外，就移动区间
        {
            if (nowTimeStamp >= mAxisX.max().toMSecsSinceEpoch())
                moveCenturalToPoint(nowTimeStamp);//区间变化的槽函数会负责更新视图
        }
    }
	finalXPosition = nowTimeStamp;//记录上一个更新视图的坐标点
}

void ChartView::changeAnotherCallout(Callout*& tempTip)
{
	if (tempTip && tempTip->isVisible())
	{
		if (tempTip->RTD_position > 0)//有对应的应答器或者RBC数据
		{
			emit AnalysisInfoClick_SIGNAL(tempTip->RTD_position);
		}
		else
		{
			l_Callout_show.push_back(tempTip);
			l_Callout_hide.pop_front();
			if (!l_Callout_hide.empty())//如果Callout池里有可用的Callout那么直接用
				tempTip = l_Callout_hide.front();
			else if (l_Callout_hide.empty() && l_Callout_hide.size() + l_Callout_show.size() < MAX_NUM_CALLOUT)//否则看是否达到了设定容量上限
			{
				l_Callout_hide.push_back(new Callout(chart()));
				tempTip = l_Callout_hide.front();
			}
			else//已到设定容量上限，动态Callout设置为NULL
				tempTip = nullptr;
		}
	}
}

bool ChartView::getDataByTimeFromFile(qint64 Time, RealTimeDatastructure* RTD)
{
	auto pos = fp_map.lowerBound(Time);
	if (pos == fp_map.end())//找不到这个元素，就直接返回错误结果
		return false;
	if (pos.key() != Time)//如果找到是第一个比目标元素大的元素，就取它前面的一个元素
	{
		if (pos == fp_map.begin())
			return false;
		pos--;
	}
	getDataFromFile(pos.key(), RTD);
	return true;
}


