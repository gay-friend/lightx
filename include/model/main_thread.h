#pragma once
#include <QGraphicsView>
#include <vector>

#include "entity/line_info.hpp"
#include "utils/utils.h"

class NodeManager
{
public:
    /// @brief 构造函数
    /// @param view 画布
    NodeManager(QGraphicsView *view);
    /// @brief 析构函数
    virtual ~NodeManager();
    /// @brief 通过端口获取与该端口连接的所有端口信息
    /// @param port 端口
    /// @return 端口信息列表(与输出端口连接可能有多个)
    std::vector<Port *> get_another_ports(Port *port);
    /// @brief 通过坐标获取端口信息
    /// @param pos 坐标
    /// @return 端口信息
    Port *get_port(QPoint pos);
    /// @brief 通过节点ID 端口ID 端口类型获取端口信息
    /// @param node_id 节点ID
    /// @param port_id 端口ID
    /// @return 端口信息
    Port *get_port(const std::string &node_id, int port_id, Port::Type port_type);
    NodeWidget *get_node(Port *port);
    /// @brief 通过坐标获取节点
    /// @param pos 坐标
    /// @return 节点
    NodeWidget *get_node(QPoint pos);
    // std::vector<Node *> get_child_nodes(Por);
    /// @brief 获取端口所有连线信息
    /// @param port 端口
    /// @return 连线信息列表
    std::vector<LineInfo> get_lines_info(Port *port);
    /// @brief 端口类型校验
    /// @param port1 端口1
    /// @param port2 端口2
    /// @return bool
    bool port_type_check(Port *port1, Port *port2);
    /// @brief 端口单调性校验
    /// @param port1 端口1
    /// @param port2 端口2
    /// @return bool
    bool port_monotonicity_check(Port *port1, Port *port2);
    /// @brief 端口数据类型校验
    /// @param port1 端口1
    /// @param port2 端口2
    /// @return bool
    bool port_data_type_check(Port *port1, Port *port2);
    /// @brief 更新坐标所在位置节点
    /// @param pos 坐标
    void update_node(QPoint pos);
    /// @brief 更新节点
    /// @param node 节点
    void update_node(NodeWidget *node);
    /// @brief 更新已选节点
    void update_selected_node();
    /// @brief 更新所有节点
    void update_all_node();
    /// @brief 添加连线
    /// @param info 线信息
    void add_relation(LineInfo info);
    /// @brief 可执行校验
    /// @return bool
    bool can_run();
    /// @brief 执行
    void run();
    /// @brief 执行节点列表
    /// @param ports_info 端口信息列表
    void node_run(std::vector<NodeWidget *> nodes);
    /// @brief 刷新节点运行标识
    void nofe_reflush();
    /// @brief 添加节点
    /// @param node 节点
    void add_node(NodeWidget *node);
    /// @brief 端口连接
    /// @param orgin_node_id 原节点ID
    /// @param orgin_port_id 原端口ID
    /// @param target_node_id 目标节点ID
    /// @param target_port_id 目标端口ID
    void port_connect(const std::string &orgin_node_id, int orgin_port_id, Port::Type origin_port_type, const std::string &target_node_id, int target_port_id, Port::Type target_port_type);
    /// @brief 端口连接
    /// @param port1 端口信息1
    /// @param port2 端口信息2
    void port_connect(Port *port1, Port *port2);
    /// @brief 端口重连
    /// @param port1 端口信息1
    /// @param port2 端口信息2
    void port_reconnect(Port *port1, Port *port2);
    /// @brief 端口类型是否可转换
    /// @param port1 端口信息1
    /// @param port2 端口信息2
    /// @return bool
    bool port_type_is_convertion(Port *port1, Port *port2);
    /// @brief 删除端口信息
    /// @param info 端口信息
    void delete_port_connect(Port *info);
    /// @brief 删除已选
    void delete_selected();
    /// @brief 获取所有节点名
    /// @return names
    std::vector<std::string> get_all_node_names();
    /// @brief 包管理
    LibManager *lib_manager;

private:
    /// @brief 线信息列表
    std::vector<LineInfo> m_lines_info;
    /// @brief 节点列表
    std::map<std::string, NodeWidget *> m_nodes_map;
    /// @brief 画布
    QGraphicsView *m_view;
};