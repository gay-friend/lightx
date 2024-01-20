#pragma once

#include <vector>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QDoubleValidator>
#include <QCheckBox>
#include <nlohmann/json.hpp>

#include "node/port.h"
#include "utils/uuid.hpp"

using json = nlohmann::json;

class Node : public QWidget
{
    Q_OBJECT
public:
    enum Type
    {
        /// @brief 相机节点
        CameraNode,
    };
    enum STATE
    {
        STATE_NORMAL,
        STATE_RUNNING,
        STATE_FINISHED,
        STATE_ERROR
    };

    virtual void init() = 0;
    virtual void uninit() = 0;
    virtual void execute() = 0;

    json to_json();
    void load_from_json(json config);

    Node(const std::string &node_name, Type node_type, QWidget *parent = nullptr);
    Port *add_port(uint id, const std::string &name, Port::Type type, Port::DataType data_type);
    void add_pair_port(uint id, const std::string &name, Port::DataType data_type, bool in_force = false);
    bool can_run() const;
    void run();
    void reset();
    /// @brief 获取端口
    /// @param port_id 端口ID
    /// @param port_type
    /// @return 端口
    Port *get_port(uint port_id, Port::Type port_type) const;
    std::vector<Port *> get_all_ports() const;
    /// @brief 获取端口
    /// @param pos 坐标
    /// @return 端口
    Port *get_port_by_pos(QPointF pos) const;
    /// @brief 获取已连接输入端口
    /// @return 端口列表
    std::vector<Port *> get_connected_in_ports() const;
    /// @brief 获取已连接输出端口
    /// @return 端口列表
    std::vector<Port *> get_connected_out_port() const;
    /// @brief 是否开始节点
    /// @return bool
    bool is_start_node() const;
    template <typename T>
    void set_port_value(uint id, Port::Type type, T value)
    {
        auto port = get_port(id, type);
        if (port == nullptr)
        {
            return;
        }
        port->set_value(value);
    }
    template <typename T>
    T get_port_value(uint id, Port::Type type) const
    {
        auto port = get_port(id, type);
        if (port == nullptr)
        {
            return T();
        }
        return port->get_value<T>();
    }

    STATE state{STATE_NORMAL};
    /// @brief 节点类型
    Type type;
    /// @brief 用户定义节点名字
    const std::string &name{""};
    /// @brief 节点ID
    std::string uuid;
    std::vector<Port *> m_ports;
    bool is_executed{false};

signals:
    void on_run_start();
    void on_run_complete();
    void on_run_error();
    void on_run_reset();

protected:
    void m_build_widget();
};

std::string get_node_type_name(Node::Type type);