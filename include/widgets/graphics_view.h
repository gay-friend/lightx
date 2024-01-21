#pragma once

#include <QGraphicsView>
#include <QWheelEvent>
#include <QAction>
#include <iostream>

#include "model/main_thread.h"
#include "widgets/bezier_curve_item.h"
#include "widgets/scene.h"
#include "widgets/tree_widget.h"

/// @brief 画布
class GraphicsView : public QGraphicsView
{
    Q_OBJECT
signals:
    void on_select(NodeWidget *w);
public slots:
    void update_selected_node();
    void add_node(NodeWidget *node_widget);
    void add_line(BezierCurveItem *line);

public:
    /// @brief 构造函数
    /// @param parent 父窗体
    GraphicsView(QWidget *parent, NodeManager *manager);
    /// @brief 鼠标滚轮事件
    /// @param event 事件源
    void wheelEvent(QWheelEvent *event) override;
    /// @brief 鼠标移动事件
    /// @param event 事件源
    void mouseMoveEvent(QMouseEvent *event) override;
    /// @brief 鼠标按下事件
    /// @param event 事件源
    void mousePressEvent(QMouseEvent *event) override;
    /// @brief 鼠标松开事件
    /// @param event 事件源
    void mouseReleaseEvent(QMouseEvent *event) override;
    /// @brief 拖动移动事件
    /// @param event 事件源
    void dragMoveEvent(QDragMoveEvent *event) override;
    /// @brief 拖动释放事件
    /// @param event 事件源
    void dropEvent(QDropEvent *event) override;
    /// @brief 按键按松开事件
    /// @param event 事件源
    void keyReleaseEvent(QKeyEvent *event) override;
    /// @brief 上下文菜单事件
    /// @param event 事件源
    void contextMenuEvent(QContextMenuEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    Port *get_port(QPoint pos);
    NodeWidget *get_node(QPoint pos);
    /// @brief 节点管理器
    NodeManager *main_thread{nullptr};

private:
    /// @brief 拖动中？
    bool m_is_drawing = false;
    /// @brief 画布拖动
    bool m_drag_mode = false;
    /// @brief 预览线
    BezierCurveItem m_preview_line{QPointF(0, 0), QPointF(0, 0)};
    /// @brief 预览线颜色
    QColor m_line_color;
    /// @brief 鼠标当前位置
    QPoint m_mouse_current_pos;
    /// @brief 鼠标点击位置
    QPoint m_mouse_clike_pos;
    /// @brief 缩放
    float m_zoom_factor{1.05f}; // 缩放
    float m_view_scale{1.0};
    float m_last_scale{0.0};
    Scene *m_scene;
};