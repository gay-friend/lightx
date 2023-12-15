#include "nodes/node.h"

Node::~Node() {}
Node::Node(const std::string &node_name, Type node_type, QPointF pos) : type(node_type), name(node_name)
{

    setPos(pos);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    title_color = m_title_color_map[node_type];
    id = m_node_id_count++;
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
    gradient.setColorAt(0, title_color);
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
    for (auto it : port_list)
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
    QGraphicsItem::mousePressEvent(event);
}

void Node::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    emit change();
    QGraphicsObject::mouseMoveEvent(event);
}

QRectF Node::boundingRect() const
{
    uint in_count = get_input_count();
    uint out_count = get_output_count();

    uint _height = std::max(in_count, out_count) * 60;
    return QRectF(0, 0, width, std::max(_height, height));
}

void Node::add_port(Port *port)
{
    port_list.push_back(port);
}

void Node::set_port_value(uint port_id, QVariant data, Port::Type type)
{
    auto i = std::find_if(port_list.begin(), port_list.end(), [type, port_id](Port *port)
                          { return port->id == port_id && port->type == type; });

    if (i != port_list.end())
    {
        Port *port = *i;
        port->data = data;
    }
}

QVariant Node::get_port_value(uint port_id, Port::Type type)
{
    return get_port(port_id, type)->data;
}

Port *Node::get_port(uint port_id, Port::Type type)
{
    auto i = std::find_if(port_list.begin(), port_list.end(), [type, port_id](Port *port)
                          { return port->id == port_id && port->type == type; });

    if (i != port_list.end())
    {
        return *i;
    }
    return nullptr;
}

void Node::set_port_data_type(uint port_id, Port::Type port_type, Port::DataType data_type)
{
    title_color = Port::PORT_COLOR_MAP[data_type];
    Port *port = get_port(port_id, port_type);
    port->data_type = data_type;
    port->update();
}

bool Node::check_port_by_pos(QPointF pos)
{
    return std::any_of(port_list.begin(), port_list.end(), [pos](Port *port)
                       { return port->rect.contains(pos); });
}

Port *Node::get_port_by_pos(QPointF pos)
{
    auto it = std::find_if(port_list.begin(), port_list.end(), [pos](Port *port)
                           { return port->rect.contains(pos); });
    if (it != port_list.end())
    {
        return *it;
    }
    return nullptr;
}

uint Node::get_input_count() const
{
    return std::count_if(port_list.begin(), port_list.end(), [](Port *port)
                         { return port->type == Port::Input || port->type == Port::InputForce; });
}

uint Node::get_output_count() const
{
    return std::count_if(port_list.begin(), port_list.end(), [](Port *port)
                         { return port->type == Port::Output; });
}

QList<Port *> Node::get_in_ports()
{
    QList<Port *> result;
    std::copy_if(
        port_list.begin(), port_list.end(), std::back_inserter(result), [](Port *port)
        { return port->type == Port::Input || port->type == Port::InputForce; });
    return result;
}

QList<Port *> Node::get_out_ports()
{
    QList<Port *> result;
    std::copy_if(port_list.begin(), port_list.end(), std::back_inserter(result), [](Port *port)
                 { return port->type == Port::Output; });
    return result;
}

QList<Port *> Node::get_connected_in_ports()
{
    QList<Port *> result;
    std::copy_if(port_list.begin(), port_list.end(), std::back_inserter(result), [](Port *port)
                 { return port->is_connected && port->type == Port::Input || port->type == Port::InputForce; });
    return result;
}

QList<Port *> Node::get_connected_out_port()
{
    QList<Port *> result;
    std::copy_if(port_list.begin(), port_list.end(), std::back_inserter(result), [](Port *port)
                 { return port->is_connected && port->type == Port::Output; });
    return result;
}

void Node::run()
{
    execute();
    is_executed = true;
}

bool Node::can_run()
{
    auto in_ports = get_in_ports();
    // 有任意强制节点未连接，则不能运行
    return !std::any_of(in_ports.begin(), in_ports.end(), [](Port *port)
                        { return (port->type == Port::InputForce) && (!port->is_connected); });
}

bool Node::is_start_node()
{
    auto in_ports = get_in_ports();
    if (in_ports.count() == 0)
    {
        return true;
    }
    // 所有输入节点未连接
    return std::all_of(in_ports.begin(), in_ports.end(), [](Port *port)
                       { return !port->is_connected; });
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