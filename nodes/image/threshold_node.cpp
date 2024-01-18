#include "nodes/threshold_node.h"

NodeInfo get_node_info()
{
    return NODE_INFO;
}
Node *create_node()
{
    return new ThresholdNode(NODE_INFO.name, NODE_INFO.type);
}

ThresholdNode::ThresholdNode(const std::string &node_name, Type node_type) : Node(node_name, node_type)
{

    add_pair_port(0, "im", Port::Image, true);
    auto port = add_port(1, "th", Port::Input, Port::Int);
    port->set_value(100);
    std::cout << port->get_value<int>() << std::endl;

    add_port(1, "out im", Port::Output, Port::Image);
    m_build_widget();
}

void ThresholdNode::init()
{
}
void ThresholdNode::uninit()
{
}
void ThresholdNode::execute()
{

    auto mat = get_port_value<cv::Mat>(0, Port::InputForce);
    auto th = get_port_value<int>(1, Port::Input);
    std::cout << get_port(1, Port::Input)->get_value<int>() << std::endl;

    if (mat.empty())
    {
        std::cout << "ThresholdNode get empty input exit..." << std::endl;
        return;
    }

    cv::Mat gray;
    if (mat.type() == CV_8UC1)
    {
        gray = mat;
    }
    else
    {
        cv::cvtColor(mat, gray, cv::COLOR_BGR2GRAY);
    }

    cv::Mat bin_mat;
    cv::threshold(gray, bin_mat, th, 255, cv::THRESH_BINARY);
    set_port_value(1, Port::Output, bin_mat);

    std::cout << "ThresholdNode running. th: " << th << std::endl;
}