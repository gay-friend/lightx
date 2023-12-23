#include "nodes/camera_node.h"
namespace fs = std::filesystem;

Node *create_node()
{
    return new CameraNode(NODE_INFO.name, NODE_INFO.type);
}
NodeInfo get_node_info()
{
    return NODE_INFO;
}

CameraNode::CameraNode(const std::string &node_name, Type node_type) : Node(node_name, node_type)
{
    add_port(0, "im", Port::Output, Port::Image);
}
void CameraNode::init()
{
}
void CameraNode::uninit()
{
}

void CameraNode::execute()
{
    // if (m_image_files.size() == 0)
    // {
    //     return;
    // }
    // m_index = m_index >= m_image_files.size() ? 0 : m_index;
    // auto filename = m_image_files[m_index];
    // auto mat = cv::imread(filename);
    // image = mat_to_qimage(mat, true);
    // QVariant val = QVariant::fromValue(mat);
    // set_port_value(0, val, Port::Output);
    // m_index++;
    // std::cout << "CameraNode running..." << std::endl;
}
