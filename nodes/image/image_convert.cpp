#include "nodes/image_convert.h"

NodeInfo get_node_info()
{
    return NODE_INFO;
}
Node *create_node(QPointF pos)
{
    return dynamic_cast<Node *>(new ImageConvertNode(pos));
}

ImageConvertNode::ImageConvertNode(QPointF pos) : Node(NODE_INFO.name, NODE_INFO.type, pos)
{

    add_port(new Port(0, "image", Port::InputForce, Port::Image));

    add_port(new Port(0, "image", Port::Output, Port::Image));
    add_port(new Port(1, "out im", Port::Output, Port::Image));
}
void ImageConvertNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Node::paint(painter, option, widget);
    // 检查图片是否有效，然后绘制它
    if (!image.isNull())
    {
        painter->drawImage(QRectF(70, 120, 80, 80), image);
    }
}

void ImageConvertNode::init()
{
}
void ImageConvertNode::uninit()
{
}
void ImageConvertNode::execute()
{
    auto in_val = get_port_value(0, Port::InputForce);
    // 将图片直接映射到输出
    set_port_value(0, in_val, Port::Output);

    auto mat = in_val.value<cv::Mat>();

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
    set_port_value(1, QVariant::fromValue(gray), Port::Output);
    // 设置显示图片
    image = mat_to_qimage(gray).copy();

    std::cout << "ImageConvertNode running..." << std::endl;
}