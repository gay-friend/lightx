#include "node/node.h"
#include <QDebug>

Port *Node::add_port(uint index, const std::string &name,
                     Port::Type type,
                     Port::DataType data_type,
                     std::vector<std::string> items)
{
    auto port = create_port(this->uuid, index, name, type, data_type, items);
    m_ports.push_back(port);
    return port;
}
Port *Node::add_pair_port(uint index, const std::string &name, Port::DataType data_type, bool in_force, std::vector<std::string> items)
{
    auto type = in_force ? Port::InputForce : Port::Input;
    auto in_port = create_port(this->uuid, index, name, type, data_type, items);
    auto out_port = create_port(this->uuid, index, name, Port::Output, data_type, items);
    out_port->set_parent(in_port);
    m_ports.push_back(in_port);
    m_ports.push_back(out_port);
    return in_port;
}

Node::Node(const std::string &node_name, Type node_type, QWidget *parent) : QWidget(parent), name(node_name), type(node_type)
{
    uuid = generate_uuid();
}
void Node::aplay()
{
    for (auto port : this->m_ports)
    {
        port->apply_backend();
    }
}
void Node::m_build_widget()
{
    auto *v_layout = new QVBoxLayout();
    auto apply_botton = new QPushButton("Apply");
    QObject::connect(apply_botton, &QPushButton::released, this, &Node::aplay);
    v_layout->addWidget(apply_botton);
    for (auto port : this->m_ports)
    {
        if (port->data_type == Port::Image || port->is_pair())
        {
            continue;
        }
        v_layout->addLayout(port->setting_layout);
    }

    for (auto port : this->m_ports)
    {
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
    for (auto port : this->m_ports)
    {
        // 有任意强制节点未连接，则不能运行
        if (port->type == Port::InputForce && !port->is_connected)
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
    for (auto port : m_ports)
    {
        if (port->uuid == uuid)
        {
            return port;
        }
    }
    return nullptr;
}

std::vector<Port *> Node::get_all_ports() const
{
    return m_ports;
}

Port *Node::get_port_by_pos(QPointF pos) const
{
    for (auto port : this->m_ports)
    {
        if (port->boundingRect().contains(pos))
        {
            return port;
        }
    }
    return nullptr;
}

std::vector<Port *> Node::get_connected_in_ports() const
{
    std::vector<Port *> ports;
    for (auto port : this->m_ports)
    {
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
    for (auto port : this->m_ports)
    {
        if (port->type == Port::Output && port->is_connected)
        {
            ports.push_back(port);
        }
    }
    return ports;
}

bool Node::is_start_node() const
{
    for (auto port : this->m_ports)
    {
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
    for (auto port : this->m_ports)
    {
        port_objs.push_back(port->dumps());
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
    for (auto port : m_ports)
    {
        port->node_id = this->uuid;
    }
    for (json port_obj : config["ports"])
    {
        for (auto port : m_ports)
        {
            int index = port_obj["index"];
            Port::Type type = port_obj["type"];
            Port::DataType data_type = port_obj["data_type"];
            if (port->index == index && port->type == type && port->data_type == data_type)
            {
                std::cout << "loading" << std::endl;
                port->loads(port_obj);
                break;
            }
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
