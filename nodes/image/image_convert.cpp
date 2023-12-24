#include "nodes/image_convert.h"

NodeInfo get_node_info()
{
    return NODE_INFO;
}
Node *create_node()
{
    return new ImageConvertNode(NODE_INFO.name, NODE_INFO.type);
}

ImageConvertNode::ImageConvertNode(const std::string &node_name, Type node_type) : Node(node_name, node_type)
{
    add_pair_port(0, "im", Port::Image, true);

    add_port(1, "res im", Port::Output, Port::Image);
}

void ImageConvertNode::init()
{
}
void ImageConvertNode::uninit()
{
}
void ImageConvertNode::execute()
{
    auto mat = get_port_value<cv::Mat>(0, Port::InputForce);

    cv::Mat gray;
    if (mat.type() == CV_8UC1)
    {
        gray = mat;
    }
    else
    {
        cv::cvtColor(mat, gray, cv::COLOR_BGR2GRAY);
    }
    // 输出灰度图
    set_port_value(1, Port::Output, gray);

    std::cout << "ImageConvertNode running..." << std::endl;
}