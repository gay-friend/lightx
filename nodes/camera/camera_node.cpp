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
    m_build_widget();

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
void CameraNode::init()
{
}
void CameraNode::uninit()
{
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
    set_port_value(0, Port::Output, mat);
    m_index++;
    std::cout << "CameraNode running..." << std::endl;
}
