#pragma once

#include <QPainter>
#include <opencv2/opencv.hpp>
#include "widgets/bezier_curve_item.h"
#include "nodes/port.h"

#ifndef NODE_API
#if __WIN32__
#define NODE_API extern "C" __declspec(dllexport)
#else
#define NODE_API extern "C"
#endif
#endif

/// @brief 节点接口类
class Node : public QGraphicsObject
{
    Q_OBJECT
public:
    /// @brief 节点类型
    enum Type
    {
        /// @brief 相机节点
        CameraNode,
    };
    /// @brief 构造函数
    /// @param node_name 节点名
    /// @param node_type 节点类型
    /// @param pos 坐标
    Node(const std::string &node_name, Type node_type, QPointF pos = QPointF(0, 0));
    /// @brief 析构函数
    virtual ~Node();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    /// @brief 鼠标按下事件
    /// @param event 时间源
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    /// @brief 鼠标移动事件
    /// @param event
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    /// @brief 边界矩形
    /// @return QRectF
    QRectF boundingRect() const;
    /// @brief 添加端口
    /// @param port 端口
    void add_port(Port *port);
    /// @brief 设置端口值
    /// @param port_id 端口ID
    /// @param data 数据
    /// @param type 端口类型
    void set_port_value(uint port_id, QVariant data, Port::Type type);
    /// @brief 获取端口值
    /// @param port_id 端口ID
    /// @param type 端口类型
    /// @return 值
    QVariant get_port_value(uint port_id, Port::Type type);
    /// @brief 设置端口数据类型
    /// @param port_id 端口ID
    /// @param port_type 端口类型
    /// @param data_type 数据类型
    void set_port_data_type(uint port_id, Port::Type port_type, Port::DataType data_type);
    /// @brief 获取端口
    /// @param port_id 端口ID
    /// @param type 端口类型
    /// @return 端口
    Port *get_port(uint port_id, Port::Type type);
    /// @brief 获取端口
    /// @param pos 坐标
    /// @return 端口
    Port *get_port_by_pos(QPointF pos);
    /// @brief 判断坐标位置是否存在端口
    /// @param pos 坐标
    /// @return bool
    bool check_port_by_pos(QPointF pos);
    /// @brief 获取输入端口数目
    /// @return uint
    uint get_input_count() const;
    /// @brief 获取输出端口数目
    /// @return uint
    uint get_output_count() const;
    /// @brief 获取输入端口
    /// @return 端口列表
    QList<Port *> get_in_ports();
    /// @brief 获取输出端口
    /// @return 端口列表
    QList<Port *> get_out_ports();
    /// @brief 获取已连接输入端口
    /// @return 端口列表
    QList<Port *> get_connected_in_ports();
    /// @brief 获取已连接输出端口
    /// @return 端口列表
    QList<Port *> get_connected_out_port();
    /// @brief 初始化
    virtual void init() = 0;
    /// @brief 反初始化
    virtual void uninit() = 0;
    /// @brief 执行
    virtual void execute() = 0;
    // 节点执行
    void run();
    /// @brief 可执行校验
    /// @return bool
    bool can_run();
    /// @brief 是否开始节点
    /// @return bool
    bool is_start_node();

    /// @brief 节点类型
    Type type;
    /// @brief 用户定义节点名字
    const std::string &name;
    /// @brief 标题颜色
    QColor title_color;
    /// @brief 高
    uint height = 80;
    /// @brief 宽
    uint width = 160;
    /// @brief 端口列表
    QList<Port *> port_list;
    /// @brief 节点ID
    uint id;
    bool is_executed{false};

private:
    /// @brief 节点ID计数
    inline static uint m_node_id_count = 0;
    /// @brief 标题颜色字典
    inline static QMap<Type, QColor> m_title_color_map{
        // {CameraNode, QColor(100, 100, 100)},
        // {StartNode, QColor(200, 0, 0)},
        // {ProgrammeControlNode, QColor(200, 100, 0)},
        // {ProgrammeLoopControlNode, QColor(200, 100, 0)},
        {CameraNode, QColor(20, 80, 150)},
        // {DataNode, QColor(220, 200, 20)},

    };
signals:
    void change(); // 节点改变信号
};

/// @brief 节点信息
typedef struct NodeInfo
{
    std::string name; // 节点名
    Node::Type type;  // 节点类型
} NodeInfo;
typedef Node *(*func_create_node)(QPointF pos);
typedef NodeInfo (*func_get_lib_name)();