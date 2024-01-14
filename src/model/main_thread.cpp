#include "model/main_thread.h"

NodeManager::NodeManager(QGraphicsView *view) : m_view(view), QThread(nullptr)
{
    lib_manager = new LibManager("lib", "create_node");
}
NodeManager::~NodeManager()
{
}
void NodeManager::run()
{
    run_once();
}
std::vector<Port *> NodeManager::get_another_ports(Port *port) const
{
    std::vector<Port *> result;
    for (auto line : m_lines_info)
    {
        if (line.is_line_port(port))
        {
            result.push_back(line.get_another_port(port));
        }
    }
    return result;
}
Port *NodeManager::get_port(QPoint pos) const
{
    QGraphicsItem *clicked_item = m_view->itemAt(pos);
    if (clicked_item != nullptr)
    {
        auto port = dynamic_cast<Port *>(clicked_item);
        return port;
    }
    return nullptr;
}
NodeWidget *NodeManager::get_node(Port *port)
{
    return m_nodes_map.count(port->node_id) == 0 ? nullptr : m_nodes_map[port->node_id];
}
NodeWidget *NodeManager::get_node(QPoint pos) const
{
    QGraphicsItem *clicked_item = m_view->itemAt(pos);
    if (clicked_item != nullptr)
    {
        NodeWidget *node = dynamic_cast<NodeWidget *>(clicked_item);
        if (node != nullptr)
        {
            return node;
        }
    }
    return nullptr;
}

std::vector<LineInfo> NodeManager::get_lines_info(Port *port) const
{
    std::vector<LineInfo> lines;
    std::copy_if(m_lines_info.begin(), m_lines_info.end(), std::back_inserter(lines), [port](const LineInfo &line)
                 { return line.is_line_port(port); });
    return lines;
}
Port *NodeManager::get_port(const std::string &node_id, int port_id, Port::Type port_type)
{
    if (m_nodes_map.count(node_id) == 0)
    {
        return nullptr;
    }
    auto node_widget = m_nodes_map[node_id];
    return node_widget->node->get_port(port_id, port_type);
}

bool NodeManager::port_type_check(Port *port1, Port *port2) const
{
    switch (port1->type)
    {
    case Port::Input:
    case Port::InputForce:
        return port2->type == Port::Output;
    case Port::Output:
        return port2->type == Port::Input || port2->type == Port::InputForce;

    default:
        return false;
    }
}
bool NodeManager::port_monotonicity_check(Port *port1, Port *port2) const
{
    auto is_input = port1->type == Port::Input || port1->type == Port::InputForce;
    if (port1->is_connected && is_input)
    {
        return false;
    }
    else if (port2->is_connected && is_input)
    {
        return false;
    }
    else if (port1->node_id == port2->node_id)
    {
        return false;
    }
    else
    {
        return true;
    }
}
bool NodeManager::port_data_type_check(Port *port1, Port *port2) const
{
    return port1->data_type == port2->data_type;
}

void NodeManager::update_node(QPoint pos)
{
    auto node = get_node(pos);
    update_node(node);
}
void NodeManager::update_node(NodeWidget *node_widget)
{
    if (node_widget == nullptr)
    {
        return;
    }

    auto uuid = node_widget->node->uuid;
    for (auto info : m_lines_info)
    {
        if (info.in_port->node_id == uuid || info.out_port->node_id == uuid)
        {
            info.line->update_point(info.in_port->get_port_pos(), info.out_port->get_port_pos());
        }
    }
}
void NodeManager::update_selected_node()
{
    for (auto item : m_view->scene()->selectedItems())
    {
        update_node(dynamic_cast<NodeWidget *>(item));
    }
}
void NodeManager::update_all_node()
{
    auto items = m_view->scene()->items();
    std::vector<QGraphicsItem *> nodes;
    std::copy_if(items.begin(), items.end(), std::back_inserter(nodes), [](QGraphicsItem *item)
                 { return dynamic_cast<NodeWidget *>(item) != nullptr; });

    for (auto node : nodes)
    {
        update_node(dynamic_cast<NodeWidget *>(node));
    }
}
void NodeManager::add_relation(LineInfo info)
{
    m_lines_info.push_back(info);
}
bool NodeManager::can_run() const
{
    for (auto item : m_nodes_map)
    {
        if (!item.second->node->can_run())
        {
            return false;
        }
    }
    return true;
}
void NodeManager::run_once()
{
    if (!can_run())
    {
        std::cout << "please connect all force in port." << std::endl;
        return;
    }
    // 重置已执行标志
    nofe_reflush();
    // 刷新场景
    m_view->scene()->update();
    // 找到开始节点
    std::vector<NodeWidget *> start_nodes;
    for (auto item : m_nodes_map)
    {
        if (item.second->node->is_start_node())
        {
            start_nodes.push_back(item.second);
        }
    }
    // 执行开始节点
    node_run(start_nodes);
}
void NodeManager::node_run(std::vector<NodeWidget *> nodes)
{
    // TODO: 使用线程池去执行
    // TODO: 实现多线分支
    for (auto node_widget : nodes)
    {
        auto node = node_widget->node;
        // 获取所有已连接的输入端口
        auto in_ports_info = node->get_connected_in_ports();
        // 获取所有未执行的父节点
        std::vector<NodeWidget *> parent_nodes;
        for (auto in_port_info : in_ports_info)
        {
            for (auto other_port : get_another_ports(in_port_info))
            {
                auto other_node = m_nodes_map[other_port->node_id];
                if (other_node->node->is_executed)
                {
                    continue;
                }
                auto it = std::find_if(parent_nodes.begin(), parent_nodes.end(), [other_node](NodeWidget *n)
                                       { return other_node->node->uuid == n->node->uuid; });
                if (it == parent_nodes.end())
                {
                    parent_nodes.push_back(other_node);
                }
            }
        }
        // 递归执行未执行的父节点
        node_run(parent_nodes);

        // 执行该节点
        if (!node->is_executed)
        {
            node->run();
        }

        // 获取所有已连接的输出端口
        auto out_ports = node->get_connected_out_port();
        // 获取所有子节点
        std::vector<NodeWidget *> child_nodes;
        for (auto out_port : out_ports)
        {
            for (auto in_port : get_another_ports(out_port))
            {
                auto it = std::find_if(child_nodes.begin(), child_nodes.end(), [in_port](NodeWidget *node)
                                       { return in_port->node_id == node->node->uuid; });
                if (it == child_nodes.end())
                {
                    child_nodes.push_back(m_nodes_map[in_port->node_id]);
                }
            }
        }
        // 递归执行子节点
        node_run(child_nodes);
    }
}
void NodeManager::nofe_reflush()
{
    for (auto node_widget : m_nodes_map)
    {
        node_widget.second->node->reset();
    }
}
void NodeManager::add_node(NodeWidget *node)
{
    if (node != nullptr)
    {
        m_nodes_map[node->node->uuid] = node;
        m_view->scene()->addItem(node);
        QObject::connect(node, &NodeWidget::on_change, [this, node]()
                         { update_selected_node(); });
    }
}
void NodeManager::port_connect(const std::string &orgin_node_id, int orgin_port_id, Port::Type origin_port_type, const std::string &target_node_id, int target_port_id, Port::Type target_port_type)
{
    auto port1 = get_port(orgin_node_id, orgin_port_id, origin_port_type);
    auto port2 = get_port(target_node_id, target_port_id, target_port_type);

    if (port1 != nullptr && port2 == nullptr)
    {
        if (port_type_check(port1, port2) && port_monotonicity_check(port1, port2) && port_data_type_check(port1, port2))
        {
            port_connect(port1, port2);
        }
    }
    m_view->scene()->update();
}
void NodeManager::port_connect(Port *port1, Port *port2)
{
    auto start = port1->get_port_pos();
    auto end = port2->get_port_pos();
    port1->connect(port2);

    auto line = new BezierCurveItem(start, end);
    line->line_color = port1->color;
    LineInfo info(port1, port2, line);
    m_view->scene()->addItem(line);
    add_relation(info);
}
void NodeManager::port_reconnect(Port *port1, Port *port2)
{
    if (port1->type == Port::Input || port1->type == Port::InputForce)
    {
        delete_port_connect(port1);
    }
    if (port2->type == Port::Input || port2->type == Port::InputForce)
    {
        delete_port_connect(port2);
    }
    port_connect(port1, port2);
}
bool NodeManager::port_type_is_convertion(Port *port1, Port *port2) const
{
    return false;
}
void NodeManager::delete_port_connect(Port *port)
{
    std::vector<LineInfo> lines;
    std::copy_if(m_lines_info.begin(), m_lines_info.end(), std::back_inserter(lines), [port](const LineInfo &lineinfo)
                 { return lineinfo.is_line_port(port); });

    for (auto line : lines)
    {
        auto out_port = line.out_port;
        m_lines_info.erase(std::find(m_lines_info.begin(), m_lines_info.end(), line));
        line.clear();

        auto it = std::find_if(m_lines_info.begin(), m_lines_info.end(), [out_port](auto lineinfo)
                               { return lineinfo.is_line_port(out_port); });
        if (it == m_lines_info.end())
        {
            out_port->disconnect();
        }
    }
    m_view->scene()->update();
}
void NodeManager::delete_selected()
{
    std::vector<BezierCurveItem *> lines;
    std::vector<NodeWidget *> nodes;
    for (auto item : m_view->scene()->selectedItems())
    {
        BezierCurveItem *line = dynamic_cast<BezierCurveItem *>(item);
        if (line != nullptr)
        {
            lines.push_back(line);
            continue;
        }
        NodeWidget *node = dynamic_cast<NodeWidget *>(item);
        if (node != nullptr)
        {
            nodes.push_back(node);
            continue;
        }
    }

    // 先删除节点的端口所有连线 和节点
    for (auto node : nodes)
    {
        for (auto port : node->node->get_all_ports())
        {
            delete_port_connect(port);
        }
        if (m_nodes_map.count(node->node->uuid))
        {
            m_nodes_map.erase(node->node->uuid);
            delete node;
        }
    }

    // 删除线
    for (auto line : lines)
    {
        // 去关系里面找
        auto it = std::find_if(m_lines_info.begin(), m_lines_info.end(), [line](LineInfo i)
                               { return i.line == line; });
        if (it != m_lines_info.end())
        {
            delete_port_connect((*it).in_port);
        }
    }
}

std::vector<std::string> NodeManager::get_all_node_names() const
{
    return lib_manager->names;
}
