#include "node/node_widget.h"

NodeWidget::NodeWidget(QGraphicsItem *parent, Node *node, QPointF pos) : node(node), QGraphicsObject(parent)
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

    uint title_width = m_title_font_size * title.length();
    m_node_width = std::max(m_node_width_min, title_width);

    m_init_node_width_height();

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

    // 连接信号槽
    QObject::connect(node, &Node::on_run_start, [this]()
                     { this->m_paint_shadow(Node::STATE_RUNNING); });
    QObject::connect(node, &Node::on_run_complete, [this]()
                     { this->m_paint_shadow(Node::STATE_FINISHED); });
    QObject::connect(node, &Node::on_run_error, [this]()
                     { this->m_paint_shadow(Node::STATE_ERROR); });
    QObject::connect(node, &Node::on_run_reset, [this]()
                     { this->m_paint_shadow(Node::STATE_NORMAL); });
}
NodeWidget::~NodeWidget() {}
void NodeWidget::m_paint_shadow(Node::STATE state)
{
    switch (state)
    {
    case Node::STATE_NORMAL:
        m_shadow->setColor("#00000000");
        setGraphicsEffect(m_shadow);
        break;
    case Node::STATE_RUNNING:
        m_shadow->setColor("#aacfcfee");
        setGraphicsEffect(m_shadow);
        break;
    case Node::STATE_FINISHED:
        m_shadow->setColor("#c4c");
        setGraphicsEffect(m_shadow);
        break;
    case Node::STATE_ERROR:
        m_shadow->setColor("#aadd1515");
        setGraphicsEffect(m_shadow);
        break;

    default:
        break;
    }

    if (isSelected())
    {
        m_shadow->setColor("#aaeeee00");
        setGraphicsEffect(m_shadow);
    }
}
void NodeWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

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

    m_paint_shadow(node->state);

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
}
void NodeWidget::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    emit on_change();
    QGraphicsObject::mouseMoveEvent(event);
}

QRectF NodeWidget::boundingRect() const
{
    return QRectF(0, 0, m_node_width, m_node_height);
}

void NodeWidget::m_init_node_width_height()
{
    auto max_in_port_width = 0;
    auto max_out_port_width = 0;
    auto in_height = 0;
    auto out_height = 0;

    auto ports = node->get_all_ports();
    for (auto port : ports)
    {
        if (port->type == Port::Output)
        {
            max_out_port_width = std::max(max_out_port_width, port->port_width);
            out_height += port->icon_size + m_port_padding;
        }
        else
        {
            max_in_port_width = std::max(max_in_port_width, port->port_width);
            in_height += port->icon_size + m_port_padding;
        }
    }

    m_node_width = std::max(m_node_width, max_in_port_width + max_out_port_width + m_port_space) + 20;

    m_node_height = std::max(out_height, in_height) + m_title_height + 10;
    m_node_height = std::max(m_node_height, m_node_height_min);
}
