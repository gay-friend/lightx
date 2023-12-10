#pragma once

#include <QGraphicsView>
#include <QWheelEvent>
#include <QAction>
#include <iostream>

#include "model/node_manager.h"
#include "widget/bezier_curve_item.h"
#include "widget/drag_label.h"

/// @brief 画布
class GraphicsView : public QGraphicsView
{
public:
    /// @brief 构造函数
    /// @param parent 父窗体
    GraphicsView(QWidget *parent = nullptr);
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
    /// @brief 拖动进入事件
    /// @param event 事件源
    void dragEnterEvent(QDragEnterEvent *event) override;
    /// @brief 拖动释放事件
    /// @param event 事件源
    void dropEvent(QDropEvent *event) override;
    /// @brief 拖动离开事件
    /// @param event 事件源
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    /// @brief 大小改变事件
    /// @param event 事件源
    void resizeEvent(QResizeEvent *event) override;
    /// @brief 视窗改变事件
    /// @param event 事件源
    bool viewportEvent(QEvent *event) override;
    /// @brief 按键按松开事件
    /// @param event 事件源
    void keyReleaseEvent(QKeyEvent *event) override;
    /// @brief 按键按下事件
    /// @param event 事件源
    void keyPressEvent(QKeyEvent *event) override;
    /// @brief 上下文菜单事件
    /// @param event 事件源
    void contextMenuEvent(QContextMenuEvent *event) override;
    /// @brief 缩放视图
    /// @param scale_factor 缩放因子
    void scaleView(qreal scale_factor);
    /// @brief 节点管理器
    NodeManager node_manager{this};

private:
    /// @brief 拖动中？
    bool m_is_drawing = false;
    /// @brief 右键按下
    bool m_right_button_pressed = false;
    /// @brief 左键按下
    bool m_left_button_pressed = false;
    /// @brief 移动场景标识
    bool m_move_view_flag = false;
    /// @brief 预览线
    BezierCurveItem m_preview_line{QPointF(0, 0), QPointF(0, 0)};
    /// @brief 预览线颜色
    QColor m_line_color;
    /// @brief 鼠标当前位置
    QPoint m_mouse_current_pos;
    /// @brief 鼠标点击位置
    QPoint m_mouse_clike_pos;
    /// @brief  鼠标释放位置
    QPoint m_mouse_release_pos;
    /// @brief 缩放
    double m_scale{1}; // 缩放
    /// @brief 鼠标滚动标识
    bool m_scoll_flag = false;
};