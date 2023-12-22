#include "nodes/split_image_node.h"

NodeInfo get_node_info()
{
    return NODE_INFO;
}
NodeWidget *create_node(QPointF pos)
{
    return dynamic_cast<NodeWidget *>(new SplitImageNode(pos));
}

SplitImageNode::SplitImageNode(QPointF pos) : NodeWidget(NODE_INFO.name, NODE_INFO.type, pos)
{

    add_port(new Port(0, "Image", Port::InputForce, Port::Image, QImage()));

    add_port(new Port(0, "Image", Port::Output, Port::Image, QImage()));
    add_port(new Port(1, "R", Port::Output, Port::Image, QImage()));
    add_port(new Port(2, "G", Port::Output, Port::Image, QImage()));
    add_port(new Port(3, "B", Port::Output, Port::Image, QImage()));
}
void SplitImageNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    NodeWidget::paint(painter, option, widget);
}

void SplitImageNode::init()
{
}
void SplitImageNode::uninit()
{
}
void SplitImageNode::execute()
{
    std::cout << "SplitImageNode running..." << std::endl;
}