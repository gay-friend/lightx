#include "nodes/port.h"

Port::Port(uint id, const std::string &name, Type type, DataType data_type, QColor color)
    : id(id), name(name), type(type), data_type(data_type), color(color), QGraphicsItem(nullptr)
{
    m_pen_default = QPen(color);
    m_pen_default.setWidthF(1.5);

    m_brush_default = QBrush(color);
    m_font = QFont("Consolas", m_font_size);
    m_label_size = QFontMetrics(m_font).horizontalAdvance(QString::fromStdString(name));
    port_width = icon_size + m_label_size;
}
QRectF Port::boundingRect() const
{
    return QRectF(0, 0, port_width, icon_size);
}

void InputPort::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // icon o-> 来表示
    if (is_connected)
    {
        painter->setPen(Qt::NoPen);
        painter->setBrush(m_brush_default);
    }
    else
    {
        painter->setPen(m_pen_default);
        painter->setBrush(Qt::NoBrush);
    }

    auto size = 0.25 * icon_size;
    painter->drawEllipse(QPointF(size, 2 * size), size, size);
    QPolygonF poly;
    poly.append(QPointF(0.6 * icon_size, 0.35 * icon_size));
    poly.append(QPointF(0.75 * icon_size, 0.5 * icon_size));
    poly.append(QPointF(0.6 * icon_size, 0.65 * icon_size));

    painter->setBrush(m_brush_default);
    painter->setPen(Qt::NoPen);
    painter->drawPolygon(poly);

    // port label
    painter->setPen(m_pen_default);
    painter->setFont(m_font);
    painter->drawText(
        QRectF(icon_size, 0, m_label_size, icon_size),
        Qt::AlignmentFlag::AlignLeft | Qt::AlignmentFlag::AlignVCenter,
        QString::fromStdString(name));
}
InputPort::InputPort(uint id, const std::string &name, Type type, DataType data_type, QColor color)
    : Port(id, name, type, data_type, color)
{
}

void OutputPort::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // port label
    painter->setPen(m_pen_default);
    painter->setFont(m_font);
    painter->drawText(
        QRectF(icon_size, 0, m_label_size, icon_size),
        Qt::AlignmentFlag::AlignRight | Qt::AlignmentFlag::AlignVCenter,
        QString::fromStdString(name));

    // icon o-> 来表示
    if (is_connected)
    {
        painter->setPen(Qt::NoPen);
        painter->setBrush(m_brush_default);
    }
    else
    {
        painter->setPen(m_pen_default);
        painter->setBrush(Qt::NoBrush);
    }

    painter->drawEllipse(
        QPointF(m_label_size + 0.5 * icon_size, 0.5 * icon_size),
        0.25 * icon_size,
        0.25 * icon_size);
    QPolygonF poly;
    poly.append(QPointF(m_label_size + 0.85 * icon_size, 0.35 * icon_size));
    poly.append(QPointF(m_label_size + 1 * icon_size, 0.5 * icon_size));
    poly.append(QPointF(m_label_size + 0.85 * icon_size, 0.65 * icon_size));

    painter->setBrush(m_brush_default);
    painter->setPen(Qt::NoPen);
    painter->drawPolygon(poly);
}
OutputPort::OutputPort(uint id, const std::string &name, Type type, DataType data_type, QColor color)
    : Port(id, name, type, data_type, color)
{
}