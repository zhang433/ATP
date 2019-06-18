#ifndef __CALLOUT_H__
#define __CALLOUT_H__
#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif
#include <QChartGlobal>
#include <QGraphicsItem>
#include <QFont>

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
QT_END_NAMESPACE

QT_CHARTS_BEGIN_NAMESPACE
class QChart;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

class Callout : public QGraphicsItem
{
public:
	Callout(QChart *parent);
	~Callout() {  };

	qint64 RTD_position;//用于指示该Callout是否有对应的应答器或者RBC信息
	void setText(const QString &text);
	void setAnchor(QPointF point);
	void updateGeometry(int x_offset, int y_offset);

	QRectF boundingRect() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

private:
	QString text;
	QRectF textRect;
	QRectF rect;
	QPointF anchor;
	QFont font;
	QChart *m_chart;
};

#endif /* __CALLOUT_H__ */
