#pragma once

#include <QPainter>
#include <opencv2/opencv.hpp>
#include <map>
#include <vector>
#include <unistd.h>

#include "widgets/bezier_curve_item.h"
#include "nodes/port.h"
#include "utils/uuid.h"

#ifndef NODE_API
#if __WIN32__
#define NODE_API extern "C" __declspec(dllexport)
#else
#define NODE_API extern "C"
#endif
#endif

class Node : public QObject
{
    Q_OBJECT
signals:
    void on_run_start();
    void on_run_complete();
    void on_run_error();
    void on_run_reset();

public:
    enum Type
    {
        /// @brief 相机节点
        CameraNode,
    };
    enum STATE
    {
        /// @brief 空闲
        NORMAL,
        /// @brief 运行中
        RUNNING,
        /// @brief 已完成
        FINISHED,
        /// @brief 错误
        ERROR
    };
    virtual void init() = 0;
    virtual void uninit() = 0;
    virtual void execute() = 0;

    Node(const std::string &node_name, Type node_type);
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

    STATE state{NORMAL};
    /// @brief 节点类型
    Type type;
    /// @brief 用户定义节点名字
    const std::string &name;
    /// @brief 节点ID
    std::string uuid;
    std::vector<Port *> m_ports;
    bool is_executed{false};
};

/// @brief 节点接口类
class NodeWidget : public QGraphicsObject
{
    Q_OBJECT
public:
    /// @brief 构造函数
    /// @param node_name 节点名
    /// @param node_type 节点类型
    /// @param pos 坐标
    NodeWidget(QGraphicsItem *parent, Node *node, QPointF pos = QPointF(0, 0));
    virtual ~NodeWidget();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    /// @brief 鼠标移动事件
    /// @param event
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    /// @brief 边界矩形
    /// @return QRectF
    QRectF boundingRect() const;
    Node *node;

private:
    void m_init_node_width_height();
    QGraphicsDropShadowEffect *m_shadow;
    /// @brief 高
    uint m_node_height{50};
    /// @brief 宽
    uint m_node_width;
    uint m_node_width_min{20};
    uint m_node_height_min{40};
    uint m_node_radius{3};
    uint m_port_padding{10};
    uint m_port_space{5};

    QBrush m_brush_title_back;
    QPen m_pen_default;
    QBrush m_brush_background{"#dd151515"};

    uint m_title_height{35};
    uint m_title_font_size{16};
    QColor m_title_color;
    uint m_title_padding{5};
    QGraphicsTextItem *m_title_item;
    /// @brief 标题颜色字典
    inline static std::map<Node::Type, QColor> m_title_color_map{
        {Node::CameraNode, QColor("#f5232e")},
        // {CameraNode, QColor("#88df00")},
        // {CameraNode, QColor("#fa8b17")},
        // {CameraNode, QColor("#4e90fe")},
    };
signals:
    void on_change(); // 节点改变信号
};

std::string get_node_type_name(Node::Type type);

/// @brief 节点信息
typedef struct NodeInfo
{
    std::string name; // 节点名
    Node::Type type;  // 节点类型
} NodeInfo;
typedef Node *(*func_create_node)();
typedef NodeInfo (*func_get_lib_name)();