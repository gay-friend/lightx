#pragma once
#include <QGraphicsView>

/// @brief 场景
class Scene : public QGraphicsScene
{
public:
    /// @brief 构造函数
    /// @param parent 父窗口
    Scene(QWidget *parent = nullptr);
    /// @brief 绘制背景
    /// @param painter 绘图类
    /// @param rect 边界
    void drawBackground(QPainter *painter, const QRectF &rect) override;

private:
    int m_scene_width{32000};
    int m_scene_height{32000};
    int m_scene_grid_size{20};
    int m_scene_grid_chunk{10};
    QPen m_norm_line_pen;
    QPen m_dark_line_pen;
};