#include "nodes/node.h"

Node::~Node() {}
Node::Node(const std::string &node_name, Type node_type, QPointF pos, bool is_preview)
    : type(node_type), name(node_name), m_is_preview_node(is_preview)
{
    // set pos, flags
    setPos(pos);
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemSendsGeometryChanges);

    // init title color
    m_title_color = m_title_color_map[node_type];
    m_pen_default = QPen(m_title_color);
    m_title_color.setAlpha(175);
    m_brush_title = QBrush(m_title_color);

    // init title
    m_title_item = new QGraphicsTextItem(this);
    m_title_item->setPlainText(QString::fromStdString(name));
    m_title_item->setFont(m_title_font);
    m_title_item->setDefaultTextColor(m_title_color);
    m_title_item->setPos(m_title_padding, m_title_padding);

    int title_width = m_title_font_size * name.length() + m_node_width_min;
    m_node_width = title_width > m_node_width ? title_width : m_node_width;

    uuid = m_node_id_count++;
}

void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    qreal corner_r = 10.0;
    qreal title_corner_r = 10.0;
    QRectF title_bar_rect(0, 0, boundingRect().width(), 40);

    if (isSelected())
    {
        QPen pen;
        pen.setColor(Qt::white);
        pen.setWidth(4);
        title_corner_r = 7;
        title_bar_rect = QRectF(2, 2, boundingRect().width() - 4, 36);
        painter->setPen(pen);
        painter->setBrush(QColor(50, 50, 50, 240));
    }
    else
    {
        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor(70, 70, 70, 240));
    }

    painter->drawRoundedRect(boundingRect(), corner_r, corner_r);
    painter->setPen(Qt::NoPen);

    QLinearGradient gradient(title_bar_rect.topLeft(), title_bar_rect.bottomRight());
    gradient.setColorAt(0, m_title_color);
    gradient.setColorAt(1, QColor(70, 70, 70, 150));
    QBrush title_brush(gradient);
    painter->setBrush(title_brush);
    painter->drawRoundedRect(title_bar_rect, title_corner_r, title_corner_r);

    // 添加标题文本
    QFont title_font = painter->font();
    title_font.setBold(true);
    title_font.setPixelSize(15);
    painter->setFont(title_font);
    painter->setPen(Qt::white);
    painter->drawText(title_bar_rect, Qt::AlignCenter, QString::fromStdString(name));

    // 画端口
    for (auto it : get_all_ports())
    {
        QPen pen;
        QPolygonF triangle;

        QColor port_color = Qt::NoBrush;
        if (it->is_connected)
        {
            port_color = it->color;
        }
        else if (it->type == Port::InputForce)
        {
            port_color = Qt::red;
        }

        pen = QPen(it->color, 5);
        pen.setCapStyle(Qt::RoundCap);
        pen.setJoinStyle(Qt::RoundJoin);
        painter->setPen(pen);
        painter->setBrush(port_color);
        painter->drawEllipse(it->rect);

        painter->setPen(Qt::white);
        // 画数据文本
        painter->drawText(it->text_rect, it->text_align, it->data.toString());

        // 画端口名字
        QFont font;
        QFont old_font = painter->font();
        painter->setFont(font);
        painter->setPen(Qt::gray);
        painter->drawText(it->name_text_rect, it->text_align, QString::fromStdString(it->name));
        painter->setFont(old_font);
    }
}
std::vector<Port *> Node::get_all_ports()
{
    std::vector<Port *> port_list;
    for (auto it : m_in_ports)
    {
        port_list.push_back(it.second);
    }
    for (auto it : m_out_ports)
    {
        port_list.push_back(it.second);
    }
    return port_list;
}

void Node::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    static uint z = 0;
    setZValue(++z);
    if (check_port_by_pos(event->pos()))
    {
        setFlag(QGraphicsItem::ItemIsMovable, false);
    }
    else
    {
        setFlag(QGraphicsItem::ItemIsMovable, true);
    }
    QGraphicsObject::mousePressEvent(event);
}

void Node::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    emit change();
    QGraphicsObject::mouseMoveEvent(event);
}

QRectF Node::boundingRect() const
{
    return QRectF(0, 0, m_node_width, m_node_height);
}

void Node::add_port(Port *port)
{
    switch (port->type)
    {
    case Port::Input:
    case Port::InputForce:
        m_in_ports[port->id] = port;
        break;
    case Port::Output:
        m_out_ports[port->id] = port;
        break;

    default:
        break;
    }
}

void Node::set_port_value(uint port_id, QVariant data, Port::Type type)
{
    auto port = get_port(port_id, type);
    if (port != nullptr)
    {
        port->data = data;
    }
}

QVariant Node::get_port_value(uint port_id, Port::Type type)
{
    auto port = get_port(port_id, type);
    if (port != nullptr)
    {
        return port->data;
    }
    return QVariant();
}

Port *Node::get_port(uint port_id, Port::Type type)
{
    auto port_list = type == Port::Output ? m_out_ports : m_in_ports;
    if (port_list.count(port_id) > 0)
    {
        return port_list[port_id];
    }
    return nullptr;
}

bool Node::check_port_by_pos(QPointF pos)
{
    return get_port_by_pos(pos) != nullptr;
}

Port *Node::get_port_by_pos(QPointF pos)
{
    for (auto it : m_in_ports)
    {
        if (it.second->rect.contains(pos))
        {
            return it.second;
        }
    }
    for (auto it : m_out_ports)
    {
        if (it.second->rect.contains(pos))
        {
            return it.second;
        }
    }
    return nullptr;
}

QList<Port *> Node::get_connected_in_ports()
{
    QList<Port *> result;
    for (auto it : m_in_ports)
    {
        if (it.second->is_connected)
        {
            result.push_back(it.second);
        }
    }
    return result;
}

QList<Port *> Node::get_connected_out_port()
{
    QList<Port *> result;
    for (auto it : m_out_ports)
    {
        if (it.second->is_connected)
        {
            result.push_back(it.second);
        }
    }
    return result;
}

void Node::run()
{
    execute();
    is_executed = true;
}

bool Node::can_run()
{
    // 有任意强制节点未连接，则不能运行
    for (auto it : m_in_ports)
    {
        if (it.second->type == Port::InputForce && !it.second->is_connected)
        {
            return false;
        }
    }
    return true;
}

bool Node::is_start_node()
{
    if (m_in_ports.size() == 0)
    {
        return true;
    }
    for (auto it : m_in_ports)
    {
        if (it.second->is_connected)
        {
            return false;
        }
    }

    return true;
}

std::string get_node_type_name(Node::Type type)
{
    switch (type)
    {
    case Node::CameraNode:
        return "Camera";

    default:
        return "Unknow";
    }
}