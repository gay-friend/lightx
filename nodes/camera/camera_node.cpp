#include "nodes/camera_node.h"
namespace fs = std::filesystem;

Node *create_node(QPointF pos)
{
    return dynamic_cast<Node *>(new CameraNode(pos));
}
NodeInfo get_node_info()
{
    return NODE_INFO;
}

void CameraNode::init()
{
}
void CameraNode::uninit()
{
}

CameraNode::CameraNode(QPointF pos) : Node(NODE_INFO.name, NODE_INFO.type, pos)
{
    add_port(new Port(0, "image", Port::Output, Port::Image));
    fs::path path(dir);
    if (!fs::exists(path))
    {
        return;
    }
    fs::directory_entry entry(path);
    if (entry.status().type() != fs::file_type::directory)
    {
        return;
    }

    for (auto it : fs::directory_iterator(dir))
    {
        auto file = it.path().generic_string();
        m_image_files.push_back(file);
    }
}
void CameraNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Node::paint(painter, option, widget);
    painter->drawText(QRectF(3, 60, 200, 40), "当前图片数量:" + QString::number(m_index) + "/" + QString::number(m_image_files.size()));
    // 检查图片是否有效，然后绘制它
    if (!image.isNull())
    {
        painter->drawImage(QRectF(3, 82, 80, 80), image);
    }
}

void CameraNode::execute()
{
    if (m_image_files.size() == 0)
    {
        return;
    }
    m_index = m_index >= m_image_files.size() ? 0 : m_index;
    auto filename = m_image_files[m_index];
    auto mat = cv::imread(filename);
    image = mat_to_qimage(mat, true);
    QVariant val = QVariant::fromValue(mat);
    set_port_value(0, val, Port::Output);
    m_index++;
    std::cout << "CameraNode running..." << std::endl;
}
