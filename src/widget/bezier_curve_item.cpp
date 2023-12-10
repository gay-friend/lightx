#include "widget/bezier_curve_item.h"

BezierCurveItem::BezierCurveItem(const QPointF &start_point, const QPointF &end_point)
    : m_start(start_point), m_end(end_point)
{
    setFlag(GraphicsItemFlag::ItemIsSelectable, true);
    setAcceptHoverEvents(true);
}

QRectF BezierCurveItem::boundingRect() const
{
    // 返回包围贝塞尔曲线的矩形
    qreal min_x = qMin(qMin(m_start.x(), m_end.x()), qMin(m_control1.x(), m_control2.x()));
    qreal min_y = qMin(qMin(m_start.y(), m_end.y()), qMin(m_control1.y(), m_control2.y()));
    qreal max_x = qMax(qMax(m_start.x(), m_end.x()), qMax(m_control1.x(), m_control2.x()));
    qreal max_y = qMax(qMax(m_start.y(), m_end.y()), qMax(m_control1.y(), m_control2.y()));
    return QRectF(min_x, min_y, max_x - min_x, max_y - min_y);
}

void BezierCurveItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    setZValue(-1);

    if (m_start.x() > m_end.x())
    {
        // 偏离起点的第一个控制点
        m_control1 = QPointF(m_start.x() - 30, m_start.y());
        // 偏离终点的第二个控制点
        m_control2 = QPointF(m_end.x() + 30, m_end.y());
    }
    else
    {
        // 偏离起点的第一个控制点
        m_control1 = QPointF(m_start.x() + 60, m_start.y());
        // 偏离终点的第二个控制点
        m_control2 = QPointF(m_end.x() - 60, m_end.y());
    }

    if (isSelected())
    {
        line_width = 15;
    }
    else
    {
        line_width = 8;
    }

    // 贝塞尔曲线
    QPainterPath bezier_path;
    bezier_path.moveTo(m_start);
    bezier_path.cubicTo(m_control1, m_control2, m_end);
    painter->setPen(QPen(line_color, line_width));
    painter->drawPath(bezier_path);
}

void BezierCurveItem::update_point(const QPointF &start_point, const QPointF &end_point)
{
    m_start = start_point;
    m_end = end_point;
    update();
}

void BezierCurveItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mousePressEvent(event);
}

void BezierCurveItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QPainterPath bezier_path;
    bezier_path.moveTo(m_start);
    bezier_path.cubicTo(m_control1, m_control2, m_end);

    if (bezier_path.intersects(QRectF(event->pos(), QSizeF(line_width, line_width))))
    {
        setSelected(true);
    }

    else
    {
        setSelected(false);
    }

    event->ignore();
}
