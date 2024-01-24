#include "node/node.h"
#include <QDebug>

Port *Node::add_port(uint index, const std::string &name,
                     Port::Type type,
                     Port::DataType data_type,
                     std::vector<std::string> items)
{
    auto port = create_port(this->uuid, index, name, type, data_type, items);
    this->m_ports_map[port->uuid] = port;
    return port;
}
Port *Node::add_pair_port(uint index, const std::string &name, Port::DataType data_type, bool in_force, std::vector<std::string> items)
{
    auto type = in_force ? Port::InputForce : Port::Input;
    auto in_port = create_port(this->uuid, index, name, type, data_type, items);
    auto out_port = create_port(this->uuid, index, name, Port::Output, data_type, items);
    out_port->set_parent(in_port);
    this->m_ports_map[in_port->uuid] = in_port;
    this->m_ports_map[out_port->uuid] = out_port;
    return in_port;
}

Node::Node(const std::string &node_name, Type node_type, QWidget *parent) : QWidget(parent), name(node_name), type(node_type)
{
    uuid = generate_uuid();
}
void Node::aplay()
{
    for (auto item : m_ports_map)
    {
        item.second->apply_backend();
    }
}
void Node::m_build_widget()
{
    auto *v_layout = new QVBoxLayout();
    auto apply_botton = new QPushButton("Apply");
    QObject::connect(apply_botton, &QPushButton::released, this, &Node::aplay);
    v_layout->addWidget(apply_botton);
    for (auto item : m_ports_map)
    {
        auto port = item.second;
        if (port->data_type == Port::Image || port->is_pair())
        {
            continue;
        }
        v_layout->addLayout(port->setting_layout);
    }

    for (auto item : m_ports_map)
    {
        auto port = item.second;
        if (port->data_type != Port::Image || port->is_pair())
        {
            continue;
        }
        v_layout->addLayout(port->setting_layout);
    }
    this->setLayout(v_layout);
}
bool Node::can_run() const
{
    for (auto item : m_ports_map)
    {
        // 有任意强制节点未连接，则不能运行
        if (item.second->type == Port::InputForce && !item.second->is_connected)
        {
            return false;
        }
    }
    return true;
}
void Node::reset()
{
    is_executed = false;
    state = STATE::STATE_NORMAL;
    emit on_run_reset();
}
void Node::run()
{
    try
    {
        state = STATE::STATE_RUNNING;
        emit on_run_start();
        execute();
        state = STATE::STATE_FINISHED;
        emit on_run_complete();
    }
    catch (const std::exception &e)
    {
        state = STATE::STATE_ERROR;
        emit on_run_error();
        std::cerr << e.what() << '\n';
    }
    is_executed = true;
}

Port *Node::get_port(std::string uuid) const
{
    if (this->m_ports_map.count(uuid) > 0)
    {
        return this->m_ports_map.at(uuid);
    }

    return nullptr;
}
std::vector<Port *> Node::get_all_ports() const
{
    std::vector<Port *> ports;
    for (auto item : this->m_ports_map)
    {
        ports.push_back(item.second);
    }
    return ports;
}

Port *Node::get_port_by_pos(QPointF pos) const
{
    for (auto item : this->m_ports_map)
    {
        if (item.second->boundingRect().contains(pos))
        {
            return item.second;
        }
    }
    return nullptr;
}

std::vector<Port *> Node::get_connected_in_ports() const
{
    std::vector<Port *> ports;
    for (auto item : this->m_ports_map)
    {
        auto port = item.second;
        if (port->type != Port::Output && port->is_connected)
        {
            ports.push_back(port);
        }
    }
    return ports;
}

std::vector<Port *> Node::get_connected_out_port() const
{
    std::vector<Port *> ports;
    for (auto item : this->m_ports_map)
    {
        auto port = item.second;
        if (port->type == Port::Output && port->is_connected)
        {
            ports.push_back(port);
        }
    }
    return ports;
}

bool Node::is_start_node() const
{
    for (auto item : this->m_ports_map)
    {
        auto port = item.second;
        if (port->type != Port::Output && port->is_connected)
        {
            return false;
        }
    }
    return true;
}
json Node::dumps()
{
    json obj;
    std::vector<json> port_objs;
    for (auto item : this->m_ports_map)
    {
        port_objs.push_back(item.second->dumps());
    }
    obj["ports"] = port_objs;
    obj["uuid"] = this->uuid;
    obj["name"] = name;
    obj["type"] = type;

    return obj;
}
void Node::loads(json config)
{
    this->uuid = config["uuid"];
    for (json port_obj : config["ports"])
    {
        auto port = get_port(port_obj["uuid"]);
        if (port != nullptr)
        {
            port->loads(port_obj);
        }
    }
}
const char *get_node_type_name(Node::Type type)
{
    switch (type)
    {
    case Node::CameraNode:
        return "Camera";

    default:
        return "Unknow";
    }
}
