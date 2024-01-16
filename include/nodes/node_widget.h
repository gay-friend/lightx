#pragma once
#include "nodes/node.h"
#include "widgets/bezier_curve_item.h"

#include <QPainter>

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
    void m_paint_shadow(Node::STATE state);
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