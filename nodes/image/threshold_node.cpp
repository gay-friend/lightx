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

    add_port(0, "image", Port::InputForce, Port::Image);
    add_port(1, "th", Port::Input, Port::Int);

    add_port(0, "image", Port::Output, Port::Image);
    add_port(1, "out im", Port::Output, Port::Image);
}

void ThresholdNode::init()
{
}
void ThresholdNode::uninit()
{
}
void ThresholdNode::execute()
{
    // auto in_val = get_port_value(0, Port::InputForce);
    // // 将图片直接映射到输出
    // set_port_value(0, in_val, Port::Output);

    // auto mat = in_val.value<cv::Mat>();
    // if (mat.empty())
    // {
    //     std::cout << "ThresholdNode get empty input exit..." << std::endl;
    //     return;
    // }

    // cv::Mat gray;
    // if (mat.type() == CV_8UC1)
    // {
    //     gray = mat;
    // }
    // else
    // {
    //     cv::cvtColor(mat, gray, cv::COLOR_BGR2GRAY);
    // }

    // cv::Mat bin_mat;
    // cv::threshold(gray, bin_mat, threshold, 255, cv::THRESH_BINARY);
    // set_port_value(1, QVariant::fromValue(bin_mat), Port::Output);
    // // 设置显示图片
    // image = mat_to_qimage(bin_mat).copy();

    // std::cout << "ThresholdNode running..." << std::endl;
}