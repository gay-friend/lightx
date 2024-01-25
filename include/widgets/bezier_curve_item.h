#pragma once

#include <QPointF>
#include <QWidget>
#include <QPainter>
#include <QGraphicsPathItem>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsDropShadowEffect>

/// @brief 贝塞尔曲线
class BezierCurveItem : public QGraphicsPathItem
{
public:
    /// @brief 构造函数
    /// @param start_point 起始点
    /// @param end_point 终止点
    BezierCurveItem(const QPointF &start_point, const QPointF &end_point);
    ~BezierCurveItem();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    /// @brief 更新
    /// @param start_point 起始点
    /// @param end_point 终止点
    void update_point(const QPointF &start_point, const QPointF &end_point);
    void update_point(const QPointF &point);

    /// @brief 线颜色
    QColor line_color{Qt::blue};

private:
    /// @brief 起始点
    QPointF m_start;
    /// @brief 终止点
    QPointF m_end;
    /// @brief 控制点1
    QPointF m_control1;
    /// @brief 控制点2
    QPointF m_control2;
    /// @brief 选中阴影
    QGraphicsDropShadowEffect *m_shadow;
    QColor m_shadow_color;
};