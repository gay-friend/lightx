#include "node/port.h"

Port::Port(const std::string &node_id, uint id, const std::string &name, Type type, DataType data_type)
    : node_id(node_id), id(id), name(name), type(type), data_type(data_type), QGraphicsObject(nullptr)
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
json Port::to_json()
{
    json config;
    config["id"] = this->id;
    config["node_id"] = this->node_id;
    config["name"] = this->name;
    config["type"] = this->type;
    config["data_type"] = this->data_type;
    switch (this->data_type)
    {
    case Port::Int:
        config["value"] = this->get_value<int>();
        break;
    case Port::Float:
        config["value"] = this->get_value<float>();
        break;
    case Port::String:
    case Port::File:
        config["value"] = this->get_value<std::string>();
        break;
    case Port::Bool:
        config["value"] = this->get_value<bool>();
        break;

    default:
        break;
    }
    return config;
}
void Port::load_from_json(json config)
{
    this->id = config["id"];
    this->node_id = config["node_id"];
    this->name = config["name"];
    this->type = config["type"];
    this->data_type = config["data_type"];
    if (config.contains("value"))
    {
        this->set_value(config["value"]);
    }
}
QPointF Port::get_port_pos() const
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
    if (type != Output && m_parent != nullptr)
    {
        QObject::disconnect(m_parent, &Port::on_value_change, this, &Port::on_value_change);
        m_parent = nullptr;
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
bool Port::readonly()
{
    return m_parent != nullptr;
}
bool Port::is_pair()
{
    return this->type == Port::Output && m_parent != nullptr;
}

void Port::set_parent(Port *port)
{
    QObject::connect(port, &Port::on_value_change, this, &Port::on_value_change);
    m_parent = port;
}
void Port::add_child(Port *port)
{
    if (port != nullptr)
    {
        this->m_childs.push_back(port);
    }
}
void Port::remove_child(Port *port)
{
    auto it = std::find(this->m_childs.begin(), this->m_childs.end(), port);
    if (it != this->m_childs.end())
    {
        m_childs.erase(it);
    }
}
QVariant *Port::get_data()
{
    return m_parent != nullptr ? m_parent->m_data : this->m_data;
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

QPointF OutputPort::get_port_pos() const
{
    auto pos = scenePos();
    return QPointF(pos.x() + m_label_size + 0.5 * icon_size, pos.y() + 0.5 * icon_size);
}