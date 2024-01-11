#include "widgets/bezier_curve_item.h"

BezierCurveItem::BezierCurveItem(const QPointF &start_point, const QPointF &end_point)
    : QGraphicsPathItem(nullptr), m_start(start_point), m_end(end_point)
{
    setZValue(-1);

    setFlags(GraphicsItemFlag::ItemIsSelectable);
    setAcceptHoverEvents(true);
    // 选中投影
    m_shadow = new QGraphicsDropShadowEffect();
    m_shadow_color = Qt::yellow;
    m_shadow->setOffset(0, 0);
    m_shadow->setBlurRadius(20);
}
BezierCurveItem ::~BezierCurveItem()
{
    if (m_shadow != nullptr)
    {
        delete m_shadow;
        m_shadow = nullptr;
    }
}

void BezierCurveItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

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

    // 贝塞尔曲线
    QPainterPath bezier_path;
    bezier_path.moveTo(m_start);
    bezier_path.cubicTo(m_control1, m_control2, m_end);
    setPath(bezier_path);

    // 绘制线
    painter->setPen(QPen(line_color, 4));
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(path());

    if (isSelected())
    {
        m_shadow->setColor(m_shadow_color);
        setGraphicsEffect(m_shadow);
    }
    else
    {
        m_shadow->setColor("#00000000");
        setGraphicsEffect(m_shadow);
    }
}

void BezierCurveItem::update_point(const QPointF &start_point, const QPointF &end_point)
{
    m_start = start_point;
    m_end = end_point;
    prepareGeometryChange();
    update();
}

void BezierCurveItem::update_point(const QPointF &point)
{
    m_start = point;
    prepareGeometryChange();
    update();
}