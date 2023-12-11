#pragma once

#include <QPointF>
#include <QWidget>
#include <QPainter>
#include <QGraphicsItem>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneHoverEvent>

/// @brief 贝塞尔曲线
class BezierCurveItem : public QGraphicsItem
{
public:
    /// @brief 构造函数
    /// @param start_point 起始点
    /// @param end_point 终止点
    BezierCurveItem(const QPointF &start_point, const QPointF &end_point);
    /// @brief 边界矩形
    /// @return QRectF
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    /// @brief 鼠标按下事件
    /// @param event 事件源
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    /// @brief 鼠标松开事件
    /// @param event 事件源
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    /// @brief 更新
    /// @param start_point 起始点
    /// @param end_point 终止点
    void update_point(const QPointF &start_point, const QPointF &end_point);

    /// @brief 线颜色
    QColor line_color{Qt::blue};
    /// @brief 线宽度
    uint line_width{8};

private:
    /// @brief 起始点
    QPointF m_start;
    /// @brief 终止点
    QPointF m_end;
    /// @brief 控制点1
    QPointF m_control1;
    /// @brief 控制点2
    QPointF m_control2;
};