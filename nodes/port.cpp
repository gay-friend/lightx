#include "nodes/port.h"

Port::Port(const std::string &node_id, uint id, const std::string &name, Type type, DataType data_type)
    : node_id(node_id), id(id), name(name), type(type), data_type(data_type), QGraphicsItem(nullptr)
{
    color = COLOR_MAP[data_type];
    m_pen_default = QPen(color);
    m_pen_default.setWidthF(1.5);
    m_brush_default = QBrush(color);
    m_font = QFont("Consolas", m_font_size);
    m_label_size = QFontMetrics(m_font).horizontalAdvance(QString::fromStdString(name));
    port_width = icon_size + m_label_size;
    m_data = new QVariant();
}
QPointF Port::get_port_pos()
{
    auto pos = scenePos();
    return QPointF(pos.x() + 0.25 * icon_size, pos.y() + 0.5 * icon_size);
}
QRectF Port::boundingRect() const
{
    return QRectF(0, 0, port_width, icon_size);
}

void Port::disconnect()
{
    is_connected = false;
    if (type != Output)
    {
        set_parent(nullptr);
    }
}
void Port::connect(Port *port)
{
    if (port == this)
    {
        return;
    }
    is_connected = true;
    port->is_connected = true;

    // this is input?
    type != Output ? set_parent(port) : port->set_parent(this);
}

void Port::set_parent(Port *port)
{
    m_parent = port;
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
InputPort::InputPort(const std::string &node_id, uint id, const std::string &name, Type type, DataType data_type)
    : Port(node_id, id, name, type, data_type)
{
}

void OutputPort::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // port label
    painter->setPen(m_pen_default);
    painter->setFont(m_font);
    painter->drawText(
        QRectF(0, 0, m_label_size, icon_size),
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
    auto size = 0.25 * icon_size;
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
OutputPort::OutputPort(const std::string &node_id, uint id, const std::string &name, Type type, DataType data_type)
    : Port(node_id, id, name, type, data_type)
{
}

QPointF OutputPort::get_port_pos()
{
    auto pos = scenePos();
    return QPointF(pos.x() + m_label_size + 0.5 * icon_size, pos.y() + 0.5 * icon_size);
}