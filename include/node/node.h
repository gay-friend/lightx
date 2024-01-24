#pragma once

#include <vector>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QDoubleValidator>
#include <QPushButton>
#include <QCheckBox>
#include <QComboBox>
#include <nlohmann/json.hpp>
#include <map>

#include "node/port.h"
#include "utils/uuid.hpp"
#include "utils/image_utils.hpp"

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

    json dumps();
    void loads(json config);

    Node(const std::string &node_name, Type node_type, QWidget *parent = nullptr);
    Port *add_port(uint index, const std::string &name, Port::Type type, Port::DataType data_type, std::vector<std::string> items={});
    Port *add_pair_port(uint index, const std::string &name, Port::DataType data_type, bool in_force, std::vector<std::string> items={});
    bool can_run() const;
    void run();
    void reset();
    /// @brief 获取端口
    /// @param port_id 端口ID
    /// @param port_type
    /// @return 端口
    Port *get_port(std::string uuid) const;
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

    STATE state{STATE_NORMAL};
    /// @brief 节点类型
    Type type;
    /// @brief 用户定义节点名字
    const std::string &name{""};
    /// @brief 节点ID
    std::string uuid;
    std::map<std::string, Port *> m_ports_map;
    bool is_executed{false};

signals:
    void on_run_start();
    void on_run_complete();
    void on_run_error();
    void on_run_reset();
public slots:
    void aplay();

protected:
    void m_build_widget();
};

const char *get_node_type_name(Node::Type type);