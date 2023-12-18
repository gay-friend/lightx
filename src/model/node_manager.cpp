#include "model/node_manager.h"

NodeManager::NodeManager(QGraphicsView *view) : m_view(view)
{
    lib_manager = new LibManager("lib", "create_node");
}
NodeManager::~NodeManager()
{
}
std::vector<PortInfo> NodeManager::get_other_ports_info(Port *port)
{
    std::vector<PortInfo> result;
    for (auto info : m_lines_info)
    {
        if (info.port_info1.port == port || info.port_info2.port == port)
        {
            result.push_back(info.get_other_port_info_by_port(port));
        }
    }
    return result;
}
PortInfo NodeManager::get_port_info(QPoint pos)
{
    PortInfo result;
    QGraphicsItem *clicked_item = m_view->itemAt(pos);
    if (clicked_item != nullptr)
    {
        Node *node = dynamic_cast<Node *>(clicked_item);
        if (node != nullptr)
        {
            Port *port = node->get_port_by_pos(node->mapFromScene(m_view->mapToScene(pos)));
            if (port != nullptr)
            {
                result.node = node;
                result.port = port;
            }
        }
    }
    return result;
}
Node *NodeManager::get_node(QPoint pos)
{
    QGraphicsItem *clicked_item = m_view->itemAt(pos);
    if (clicked_item != nullptr)
    {
        Node *node = dynamic_cast<Node *>(clicked_item);
        if (node != nullptr)
        {
            return node;
        }
    }
    return nullptr;
}

std::vector<LineInfo> NodeManager::get_lines_info(Port *port)
{
    std::vector<LineInfo> result;
    for (auto info : m_lines_info)
    {
        if (info.port_info1.port == port || info.port_info2.port == port)
        {
            result.push_back(info);
        }
    }
    return result;
}
PortInfo NodeManager::get_port_info(int node_id, int port_id, int port_type)
{
    PortInfo result;
    auto it = std::find_if(m_nodes.begin(), m_nodes.end(), [node_id](Node *node)
                           { return node_id == node->uuid; });
    if (it != m_nodes.end())
    {
        auto node = *it;
        auto port_list = node->get_all_ports();
        auto port_it = std::find_if(port_list.begin(), port_list.end(), [port_id](Port *port)
                                    { return port->id = port_id; });
        if (port_it != port_list.end())
        {
            result.node = node;
            result.port = *port_it;
        }
    }
    return result;
}

bool NodeManager::port_type_check(PortInfo info1, PortInfo info2)
{
    if (!info1.port->port_type_check(info2.port->type))
    {
        return false;
    }
    if (info1.node == info2.node)
    {
        return false;
    }
    return true;
}
bool NodeManager::port_monotonicity_check(PortInfo info1, PortInfo info2)
{
    auto is_input = info1.port->type == Port::Input || info1.port->type == Port::InputForce;
    if (info1.port->is_connected && is_input)
    {
        return false;
    }
    else if (info2.port->is_connected && is_input)
    {
        return false;
    }
    else
    {
        return true;
    }
}
bool NodeManager::port_data_type_check(PortInfo info1, PortInfo info2)
{
    return info1.port->data_type == info2.port->data_type;
}

void NodeManager::update_node(QPoint pos)
{
    auto node = get_node(pos);
    update_node(node);
}
void NodeManager::update_node(Node *node)
{
    if (node == nullptr)
    {
        return;
    }
    std::vector<LineInfo> infos;
    std::copy_if(m_lines_info.begin(), m_lines_info.end(), std::back_inserter(infos), [node](LineInfo info)
                 { return info.port_info1.node == node || info.port_info2.node == node; });
    for (auto info : infos)
    {
        auto start = info.port_info1.node->mapToScene(info.port_info1.port->rect.center());
        auto end = info.port_info2.node->mapToScene(info.port_info2.port->rect.center());
        info.port_info1.port->is_connected = true;
        info.port_info2.port->is_connected = true;
        info.line->update_point(start, end);
    }
}
void NodeManager::update_selected_node()
{
    auto items = m_view->scene()->selectedItems();
    if (items.count() == 0)
    {
        return;
    }
    std::vector<QGraphicsItem *> nodes;
    std::copy_if(items.begin(), items.end(), std::back_inserter(nodes), [](QGraphicsItem *item)
                 { return dynamic_cast<Node *>(item) != nullptr; });

    for (auto node : nodes)
    {
        update_node(dynamic_cast<Node *>(node));
    }
}
void NodeManager::update_all_node()
{
    auto items = m_view->scene()->items();
    std::vector<QGraphicsItem *> nodes;
    std::copy_if(items.begin(), items.end(), std::back_inserter(nodes), [](QGraphicsItem *item)
                 { return dynamic_cast<Node *>(item) != nullptr; });

    for (auto node : nodes)
    {
        update_node(dynamic_cast<Node *>(node));
    }
}
void NodeManager::add_relation(LineInfo info)
{
    m_lines_info.push_back(info);
}
bool NodeManager::can_run()
{
    if (m_nodes.size() == 0)
    {
        return false;
    }
    return !std::any_of(m_nodes.begin(), m_nodes.end(), [](Node *node)
                        { return !node->can_run(); });
}
void NodeManager::run()
{
    if (!can_run())
    {
        std::cout << "please connect all force in port." << std::endl;
        return;
    }
    // 找到开始节点
    std::vector<Node *> start_nodes;
    std::copy_if(m_nodes.begin(), m_nodes.end(), std::back_inserter(start_nodes), [](Node *node)
                 { return node->is_start_node(); });

    // 执行开始节点
    node_run(start_nodes);

    // 重置已执行标志
    nofe_reflush();
    // 刷新场景
    m_view->scene()->update();
}
void NodeManager::node_run(std::vector<Node *> nodes)
{
    // TODO: 使用线程池去执行
    // TODO: 实现多线分支
    for (auto node : nodes)
    {
        // 获取所有已连接的输入端口
        auto in_ports_info = node->get_connected_in_ports();
        // 获取所有未执行的父节点
        std::vector<Node *> parent_nodes;
        for (auto in_port_info : in_ports_info)
        {
            for (auto other_port_info : get_other_ports_info(in_port_info))
            {
                if (other_port_info.node->is_executed)
                {
                    continue;
                }
                auto it = std::find_if(parent_nodes.begin(), parent_nodes.end(), [other_port_info](Node *n)
                                       { return other_port_info.node->uuid == n->uuid; });
                if (it == parent_nodes.end())
                {
                    parent_nodes.push_back(other_port_info.node);
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
        auto out_ports_info = node->get_connected_out_port();
        // 获取所有子节点
        std::vector<Node *> child_nodes;
        for (auto out_port_info : out_ports_info)
        {
            for (auto other_port_info : get_other_ports_info(out_port_info))
            {
                // 给子节点输入端口赋值
                other_port_info.port->set_port_value(out_port_info);

                auto it = std::find_if(child_nodes.begin(), child_nodes.end(), [other_port_info](Node *node)
                                       { return other_port_info.node->uuid == node->uuid; });
                if (it == child_nodes.end())
                {
                    child_nodes.push_back(other_port_info.node);
                }
            }
        }
        // 递归执行子节点
        node_run(child_nodes);
    }
}
void NodeManager::nofe_reflush()
{
    for (auto node : m_nodes)
    {
        node->is_executed = false;
    }
}
void NodeManager::add_node(Node *node)
{
    if (node != nullptr)
    {
        m_nodes.push_back(node);
        m_view->scene()->addItem(node);
        QObject::connect(node, &Node::change, [this, node]()
                         { update_selected_node(); });
    }
}
void NodeManager::port_connect(int orgin_node_id, int orgin_port_id, int orgin_port_type, int target_node_id, int target_port_id, int target_port_type)
{
    auto info1 = get_port_info(orgin_node_id, orgin_port_type, orgin_port_type);
    auto info2 = get_port_info(target_node_id, target_port_id, target_port_type);

    if (!info1.is_empty() && info2.is_empty())
    {
        if (port_type_check(info1, info2) && port_monotonicity_check(info1, info2) && port_data_type_check(info1, info2))
        {
            port_connect(info1, info2);
        }
    }
}
void NodeManager::port_connect(PortInfo info1, PortInfo info2)
{
    QPointF start(info1.node->mapToScene(info1.port->rect.center()));
    QPointF end(info2.node->mapToScene(info2.port->rect.center()));
    BezierCurveItem *curve_item = new BezierCurveItem(start, end);
    curve_item->line_color = info1.port->color;
    LineInfo info;
    info.line = curve_item;
    info.port_info1 = info1;
    info.port_info2 = info2;
    m_view->scene()->addItem(curve_item);
    add_relation(info);
    info1.port->is_connected = true;
    info2.port->is_connected = true;
}
void NodeManager::port_reconnect(PortInfo info1, PortInfo info2)
{
    if (info1.port->type == Port::Input || info1.port->type == Port::InputForce)
    {
        delete_port_connect(info1);
    }
    if (info2.port->type == Port::Input || info2.port->type == Port::InputForce)
    {
        delete_port_connect(info2);
    }
    port_connect(info1, info2);
}
bool NodeManager::port_type_is_convertion(PortInfo info1, PortInfo info2)
{
    return false;
}
void NodeManager::delete_port_connect(PortInfo info)
{
    auto it = std::find_if(m_lines_info.begin(), m_lines_info.end(), [info](const LineInfo &lineinfo)
                           { return lineinfo.port_info1.port == info.port || lineinfo.port_info2.port == info.port; });

    if (it != m_lines_info.end())
    {
        auto line = *it;
        line.delete_line();
        // 删除找到连线信息
        m_lines_info.erase(it);
    }
}
void NodeManager::delete_selected()
{
    std::vector<BezierCurveItem *> lines;
    std::vector<Node *> nodes;
    for (auto item : m_view->scene()->selectedItems())
    {
        BezierCurveItem *line = dynamic_cast<BezierCurveItem *>(item);
        if (line != nullptr)
        {
            lines.push_back(line);
            continue;
        }
        Node *node = dynamic_cast<Node *>(item);
        if (node != nullptr)
        {
            nodes.push_back(node);
            continue;
        }
    }

    // 先删除节点的端口所有连线 和节点
    for (auto node : nodes)
    {
        for (auto port : node->get_all_ports())
        {
            PortInfo portinfo;
            portinfo.node = node;
            portinfo.port = port;
            delete_port_connect(portinfo);
        }
        auto it = std::find(m_nodes.begin(), m_nodes.end(), node);
        m_nodes.erase(it);
        delete node;
        node = nullptr;
    }

    // 删除线
    for (auto line : lines)
    {
        if (line != nullptr)
        {
            // 去关系里面找
            for (auto lineinfo : m_lines_info)
            {
                if (lineinfo.line == line)
                {
                    delete_port_connect(lineinfo.port_info1);
                }
            }
        }
    }
}

std::vector<std::string> NodeManager::get_all_node_names()
{
    return lib_manager->names;
}
