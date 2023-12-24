#include "nodes/split_image_node.h"

NodeInfo get_node_info()
{
    return NODE_INFO;
}
Node *create_node()
{
    return new SplitImageNode(NODE_INFO.name, NODE_INFO.type);
}

SplitImageNode::SplitImageNode(const std::string &node_name, Type node_type) : Node(node_name, node_type)
{

    add_port(0, "im", Port::InputForce, Port::Image);

    add_port(0, "im", Port::Output, Port::Image);
    add_port(1, "R", Port::Output, Port::Image);
    add_port(2, "G", Port::Output, Port::Image);
    add_port(3, "B", Port::Output, Port::Image);
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