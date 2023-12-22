#include "nodes/node.h"

#ifndef NODE_API
#if __WIN32__
#define NODE_API extern "C" __declspec(dllexport)
#else
#define NODE_API extern "C"
#endif
#endif

void Node::add_port(uint id, const std::string &name, Port::Type type, Port::DataType data_type, QColor color)
{
    switch (type)
    {
    case Port::Input:
    case Port::InputForce:
        m_ports.push_back(new InputPort(id, name, type, data_type, color));
        break;
    default:
        m_ports.push_back(new OutputPort(id, name, type, data_type, color));
        break;
    }
}

Node::Node(const std::string &node_name, Type node_type) : name(node_name), type(node_type)
{
    uuid = generate_uuid();
}
bool Node::can_run()
{
    for (auto port : m_ports)
    {
        // 有任意强制节点未连接，则不能运行
        if (port->type == Port::InputForce && !port->is_connected)
        {
            return false;
        }
    }
    return true;
}
void Node::run()
{
    execute();
    is_executed = true;
}

Port *Node::get_port(uint port_id, Port::Type port_type)
{
    for (auto port : m_ports)
    {
        if (port->id == port_id && port->type == port_type)
        {
            return port;
        }
    }
    return nullptr;
}
std::vector<Port *> Node::get_all_ports()
{
    return m_ports;
}

Port *Node::get_port_by_pos(QPointF pos)
{
    for (auto port : m_ports)
    {
        if (port->boundingRect().contains(pos))
        {
            return port;
        }
    }
    return nullptr;
}

std::vector<Port *> Node::get_connected_in_ports()
{
    std::vector<Port *> ports;
    for (auto port : m_ports)
    {
        if (port->type != Port::Output && port->is_connected)
        {
            ports.push_back(port);
        }
    }
    return ports;
}

std::vector<Port *> Node::get_connected_out_port()
{
    std::vector<Port *> ports;
    for (auto port : m_ports)
    {
        if (port->type == Port::Output && port->is_connected)
        {
            ports.push_back(port);
        }
    }
    return ports;
}

bool Node::is_start_node()
{
    for (auto port : m_ports)
    {
        if (port->type != Port::Output && port->is_connected)
        {
            return false;
        }
    }
    return true;
}

NodeWidget::NodeWidget(Node *node, QPointF pos) : node(node)
{
    // set pos, flags
    setPos(pos);
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemSendsGeometryChanges);

    // init title color
    m_title_color = m_title_color_map[node->type];
    m_pen_default = QPen(m_title_color);
    m_title_color.setAlpha(175);
    m_brush_title_back = QBrush(m_title_color);

    // init title
    auto title = QString::fromStdString(node->name);
    m_title_item = new QGraphicsTextItem(this);
    m_title_item->setPlainText(title);
    m_title_item->setFont(QFont("Arial", m_title_font_size));
    m_title_item->setDefaultTextColor(m_title_color);
    m_title_item->setPos(m_title_padding, m_title_padding);

    int title_width = m_title_font_size * title.length() + m_node_width_min;
    m_node_width = title_width > m_node_width ? title_width : m_node_width;

    // m_init_node_width_height();
    // init_ports();

    // 选中投影
    m_shadow = new QGraphicsDropShadowEffect();
    m_shadow->setOffset(0, 0);
    m_shadow->setBlurRadius(20);

    for (auto port : node->get_all_ports())
    {
        port->setParentItem(this);
        auto x = port->type == Port::Output ? m_node_width - port->port_width - m_port_padding : m_port_padding;
        auto y = m_title_height + port->id * (m_port_padding + port->icon_size) + m_port_padding;
        port->setPos(x, y);
    }
}
NodeWidget::~NodeWidget() {}
void NodeWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    switch (node->state)
    {
    case Node::NORMAL:
        m_shadow->setColor("#00000000");
        break;
    case Node::RUNNING:
        m_shadow->setColor("#aacfcfee");
        break;
    case Node::FINISHED:
        m_shadow->setColor("#c4c");
        break;
    case Node::ERROR:
        m_shadow->setColor("#aadd1515");
        break;

    default:
        break;
    }

    if (isSelected())
    {
        m_shadow->setColor("#aaeeee00");
        setGraphicsEffect(m_shadow);
    }

    // 画背景颜色
    QPainterPath node_outline;
    node_outline.addRoundedRect(
        0, 0, m_node_width, m_node_height, m_node_radius, m_node_radius);
    painter->setPen(Qt::NoPen);
    painter->setBrush(m_brush_background);
    painter->drawPath(node_outline.simplified());
    // draw outline
    isSelected() ? painter->setPen(QPen(QColor("#ddffee00"))) : painter->setPen(m_pen_default);
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(node_outline);

    // 画title的背景颜色
    QPainterPath title_outline;
    title_outline.setFillRule(Qt::WindingFill);
    title_outline.addRoundedRect(
        0,
        0,
        m_node_width,
        m_title_height,
        m_node_radius,
        m_node_radius);
    title_outline.addRoundedRect(
        0,
        m_title_height - m_node_radius,
        m_node_radius,
        m_node_radius,
        0,
        0);
    title_outline.addRoundedRect(
        m_node_width - m_node_radius,
        m_title_height - m_node_radius,
        m_node_radius,
        m_node_radius,
        0,
        0);

    painter->setPen(Qt::NoPen);
    painter->setBrush(m_brush_title_back);
    painter->drawPath(title_outline.simplified());

    // 画端口
    // for (auto it : get_all_ports())
    // {
    //     QPen pen;
    //     QPolygonF triangle;

    //     QColor port_color = Qt::NoBrush;
    //     if (it->is_connected)
    //     {
    //         port_color = it->color;
    //     }
    //     else if (it->type == Port::InputForce)
    //     {
    //         port_color = Qt::red;
    //     }

    //     pen = QPen(it->color, 5);
    //     pen.setCapStyle(Qt::RoundCap);
    //     pen.setJoinStyle(Qt::RoundJoin);
    //     painter->setPen(pen);
    //     painter->setBrush(port_color);
    //     painter->drawEllipse(it->rect);

    //     painter->setPen(Qt::white);
    //     // 画数据文本
    //     painter->drawText(it->text_rect, it->text_align, it->data.toString());

    //     // 画端口名字
    //     QFont font;
    //     QFont old_font = painter->font();
    //     painter->setFont(font);
    //     painter->setPen(Qt::gray);
    //     painter->drawText(it->name_text_rect, it->text_align, QString::fromStdString(it->name));
    //     painter->setFont(old_font);
    // }
}

void NodeWidget::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    // static uint z = 0;
    // setZValue(++z);
    setFlag(QGraphicsItem::ItemIsMovable, node->get_port_by_pos(event->pos()) == nullptr);
    QGraphicsObject::mousePressEvent(event);
}

void NodeWidget::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    emit change();
    QGraphicsObject::mouseMoveEvent(event);
}

QRectF NodeWidget::boundingRect() const
{
    return QRectF(0, 0, m_node_width, m_node_height);
}

Node *node;

void NodeWidget::m_init_node_width_height() {}

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
