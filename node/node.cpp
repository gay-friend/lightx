#include "node/node.h"

Port *Node::add_port(uint id, const std::string &name, Port::Type type, Port::DataType data_type)
{
    Port *port;
    switch (type)
    {
    case Port::Input:
    case Port::InputForce:
        port = new InputPort(uuid, id, name, type, data_type);
        break;
    default:
        port = new OutputPort(uuid, id, name, type, data_type);
        break;
    }
    m_ports.push_back(port);
    return port;
}
void Node::add_pair_port(uint id, const std::string &name, Port::DataType data_type, bool in_force)
{
    auto type = in_force ? Port::InputForce : Port::Input;
    auto in_port = new InputPort(uuid, id, name, type, data_type);
    auto out_port = new OutputPort(uuid, id, name, Port::Output, data_type);
    out_port->set_parent(in_port);
    m_ports.push_back(in_port);
    m_ports.push_back(out_port);
}

Node::Node(const std::string &node_name, Type node_type, QWidget *parent) : QWidget(parent), name(node_name), type(node_type)
{
    uuid = generate_uuid();
}
void Node::m_build_widget()
{
    auto *v_layout = new QVBoxLayout();
    for (auto port : m_ports)
    {
        if (port->data_type == Port::Image)
        {
            continue;
        }

        if (port->data_type != Port::Bool)
        {
            auto *h_layout = new QHBoxLayout();
            auto label = new QLabel(QString::fromStdString(port->name));
            h_layout->addWidget(label);
            auto edit = new QLineEdit();
            edit->setReadOnly(port->readonly());
            edit->setText(port->get_data()->toString());
            std::cout << port->get_value<std::string>() << std::endl;
            if (port->data_type == Port::Int)
            {
                edit->setValidator(new QIntValidator());
            }
            else if (port->data_type == Port::Float)
            {
                edit->setValidator(new QDoubleValidator());
            }

            h_layout->addWidget(edit);
            v_layout->addLayout(h_layout);
        }
        else
        {
            auto check = new QCheckBox(QString::fromStdString(port->name));
            v_layout->addWidget(check);
        }
    }

    for (auto port : m_ports)
    {
        if (port->data_type != Port::Image || port->is_pair())
        {
            continue;
        }
        auto label = new QLabel(QString::fromStdString(port->name));
        auto im_label = new QLabel();
        QObject::connect(port, &Port::on_value_change, [im_label](QVariant *value)
                         { 
                            auto mat = value->value<cv::Mat>();
                            auto qim = mat_to_qimage(mat); 
                            im_label->setPixmap(QPixmap::fromImage(qim)); });
        v_layout->addWidget(label);
        v_layout->addWidget(im_label);
    }
    this->setLayout(v_layout);
}
bool Node::can_run() const
{
    for (auto port : m_ports)
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
    state = STATE::NORMAL;
    emit on_run_reset();
}
void Node::run()
{
    try
    {
        state = STATE::RUNNING;
        emit on_run_start();
        execute();
        state = STATE::FINISHED;
        emit on_run_complete();
    }
    catch (const std::exception &e)
    {
        state = STATE::ERROR;
        emit on_run_error();
        std::cerr << e.what() << '\n';
    }
    is_executed = true;
}

Port *Node::get_port(uint port_id, Port::Type port_type) const
{
    for (auto port : m_ports)
    {
        if (port->id == port_id && port->type == port_type)
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
    for (auto port : m_ports)
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
    for (auto port : m_ports)
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
    for (auto port : m_ports)
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
    for (auto port : m_ports)
    {
        if (port->type != Port::Output && port->is_connected)
        {
            return false;
        }
    }
    return true;
}
json Node::to_json()
{
    json obj;
    std::vector<json> port_objs;
    for (auto port : m_ports)
    {
        port_objs.push_back(port->to_json());
    }
    obj["ports"] = port_objs;
    obj["uuid"] = this->uuid;
    obj["name"] = name;
    obj["type"] = type;

    return obj;
}
void Node::load_from_json(json config)
{
    this->uuid = config["uuid"];
    for (json port_obj : config["ports"])
    {
        auto port = get_port(port_obj["id"], port_obj["type"]);
        if (port != nullptr)
        {
            port->load_from_json(port_obj);
        }
    }
}
std::string get_node_type_name(Node::Type type)
{
    switch (type)
    {
    case Node::CameraNode:
        return "Camera";

    default:
        return "Unknow";
    }
}
