#include "model/main_thread.h"

NodeManager::NodeManager() : QThread(nullptr)
{
    lib_manager = new LibManager("lib");
}
NodeManager::~NodeManager()
{
}
void NodeManager::run()
{
    save_workspace("workspace.json");
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

NodeWidget *NodeManager::get_node(Port *port)
{
    return m_nodes_map.count(port->node_id) == 0 ? nullptr : m_nodes_map[port->node_id];
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
void NodeManager::save_workspace(const std::string &file)
{
    json workspace;
    std::vector<json> widget_objs;
    for (auto item : m_nodes_map)
    {
        json w_obj;
        auto node_widget = item.second;
        w_obj["x"] = node_widget->pos().x();
        w_obj["y"] = node_widget->pos().y();
        w_obj["node"] = node_widget->node->to_json();
        widget_objs.push_back(w_obj);
    }
    workspace["widgets"] = widget_objs;

    std::vector<json> line_objs;
    for (auto line : m_lines_info)
    {
        json line_obj;
        line_obj["target_port_id"] = line.in_port->id;
        line_obj["target_port_type"] = line.in_port->type;
        line_obj["target_node_id"] = line.in_port->node_id;

        line_obj["origin_port_id"] = line.out_port->id;
        line_obj["origin_port_type"] = line.out_port->type;
        line_obj["origin_node_id"] = line.out_port->node_id;
        line_objs.push_back(line_obj);
    }
    workspace["lines"] = line_objs;

    std::ofstream f;
    f.open(file);
    f << workspace;
    f.close();
}
void NodeManager::load_workspace(const std::string &file)
{
    try
    {
        std::ifstream f(file);
        json workspace = json::parse(f);
        for (json obj : workspace["widgets"])
        {
            json node_obj = obj["node"];

            auto node_type_name = get_node_type_name(node_obj["type"]);
            auto node_name = node_obj["name"];
            auto node = this->lib_manager->create_node(node_type_name, node_name);
            if (node != nullptr)
            {
                auto node = lib_manager->create_node(node_type_name, node_name);
                node->load_from_json(node_obj);
                auto node_widget = new NodeWidget(nullptr, node, QPointF(obj["x"], obj["y"]));
                this->add_node(node_widget);
                emit on_node_add(node_widget);
            }
        }

        for (json line_obj : workspace["lines"])
        {
            std::string target_node_id = line_obj["target_node_id"];
            int target_port_id = line_obj["target_port_id"];
            Port::Type target_port_type = line_obj["target_port_type"];

            std::string origin_node_id = line_obj["origin_node_id"];
            int origin_port_id = line_obj["origin_port_id"];
            Port::Type origin_port_type = line_obj["origin_port_type"];
            port_connect(origin_node_id, origin_port_id, origin_port_type, target_node_id, target_port_id, target_port_type);
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}
void NodeManager::add_node(NodeWidget *node)
{
    m_nodes_map[node->node->uuid] = node;
}
void NodeManager::create_node(const std::string &node_type, const std::string &node_name, QPointF pos)
{
    auto node = this->lib_manager->create_node(node_type, node_name);
    if (node != nullptr)
    {
        auto node_widget = new NodeWidget(nullptr, node, pos);
        this->add_node(node_widget);
        emit on_node_add(node_widget);
    }
}
void NodeManager::port_connect(const std::string &orgin_node_id, int orgin_port_id, Port::Type origin_port_type, const std::string &target_node_id, int target_port_id, Port::Type target_port_type)
{
    auto port1 = get_port(orgin_node_id, orgin_port_id, origin_port_type);
    auto port2 = get_port(target_node_id, target_port_id, target_port_type);
    port_connect(port1, port2);
}
void NodeManager::port_connect(Port *port1, Port *port2)
{
    if (port1 == nullptr || port2 == nullptr)
    {
        return;
    }
    if (!this->port_type_check(port1, port2))
    {
        return;
    }
    if (!this->port_data_type_check(port1, port2))
    {
        return;
    }
    if (!this->port_monotonicity_check(port1, port2))
    {
        this->port_reconnect(port1, port2);
        return;
    }
    auto start = port1->get_port_pos();
    auto end = port2->get_port_pos();
    port1->connect(port2);

    auto line = new BezierCurveItem(start, end);
    line->line_color = port1->color;
    LineInfo info(port1, port2, line);
    emit on_line_add(line);
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
    emit this->on_scene_update();
}
void NodeManager::delete_selected(QList<QGraphicsItem *> select_items)
{
    std::vector<BezierCurveItem *> lines;
    std::vector<NodeWidget *> nodes;
    for (auto item : select_items)
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