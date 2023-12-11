#include "widgets/scene.h"

Scene::Scene(QWidget *parent) : QGraphicsScene(parent)
{
    // 设置背景画刷
    setBackgroundBrush(QBrush(QColor("#212121")));
    // 设置背景大小
    setSceneRect(-m_scene_width / 2, -m_scene_height / 2, m_scene_width, m_scene_width);
    // 普通画笔
    m_norm_line_pen = QPen(QColor("#313131"));
    m_norm_line_pen.setWidthF(1.0);
    // 暗色画笔
    m_dark_line_pen = QPen(QColor("#151515"));
    m_norm_line_pen.setWidthF(1.5);

    setItemIndexMethod(QGraphicsScene::NoIndex);
}

void Scene::drawBackground(QPainter *painter, const QRectF &rect)
{
    QGraphicsScene::drawBackground(painter, rect);
    int left = rect.left();
    int right = rect.right();
    int top = rect.top();
    int bottom = rect.bottom();
    int first_left = left - (left % m_scene_grid_size);
    int first_top = top - (top % m_scene_grid_size);

    QList<QLine> lines;
    QList<QLine> dark_lines;
    int step = m_scene_grid_size * m_scene_grid_size;
    // 画横线
    for (int v = first_top; v <= bottom; v += m_scene_grid_size)
    {
        if (v % step == 0)
        {
            dark_lines.append((QLine(left, v, right, v)));
        }
        else
        {
            lines.append((QLine(left, v, right, v)));
        }
    }
    // 画竖线
    for (int h = first_left; h <= right; h += m_scene_grid_size)
    {
        if (h % step == 0)
        {
            dark_lines.append((QLine(h, top, h, bottom)));
        }
        else
        {
            lines.append((QLine(h, top, h, bottom)));
        }
    }

    painter->setPen(m_norm_line_pen);
    painter->drawLines(lines);

    painter->setPen(m_dark_line_pen);
    painter->drawLines(dark_lines);
}