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

    add_port(0, "im", Port::InputForce, Port::Image);

    add_port(0, "im", Port::Output, Port::Image);
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
    // auto in_val = get_port_value(0, Port::InputForce);
    // // 将图片直接映射到输出
    // set_port_value(0, in_val, Port::Output);

    // auto mat = in_val.value<cv::Mat>();

    // cv::Mat gray;
    // if (mat.type() == CV_8UC1)
    // {
    //     gray = mat;
    // }
    // else
    // {
    //     cv::cvtColor(mat, gray, cv::COLOR_BGR2GRAY);
    // }
    // // 输出灰度图
    // set_port_value(1, QVariant::fromValue(gray), Port::Output);
    // // 设置显示图片
    // image = mat_to_qimage(gray).copy();

    // std::cout << "ImageConvertNode running..." << std::endl;
}