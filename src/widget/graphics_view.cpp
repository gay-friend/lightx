#include "widget/graphics_view.h"

GraphicsView::GraphicsView(QWidget *parent) : QGraphicsView(parent)
{
    // 设置框选模式
    setRubberBandSelectionMode(Qt::IntersectsItemBoundingRect);
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing | QPainter::LosslessImageRendering);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // 设置缓冲背景 加速渲染
    setCacheMode(QGraphicsView::CacheBackground);
    setDragMode(GraphicsView::ScrollHandDrag); // 设置这里可以拖动
    setMouseTracking(true);                    // 跟踪鼠标位置
    // setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // 隐藏水平条
    // setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);   // 隐藏竖条
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);

    // 启用框选功能
    setDragMode(QGraphicsView::RubberBandDrag);

    auto *scene = new QGraphicsScene();
    setScene(scene);
    // 添加连续预览线到场景
    scene->addItem(&m_preview_line);
    scene->setBackgroundBrush(QColor(192, 192, 192));
    m_preview_line.setVisible(false);
}

void GraphicsView::wheelEvent(QWheelEvent *event)
{
    auto angle_scale = event->angleDelta().y();
    if (!m_scoll_flag)
    {
        resetTransform();
        return;
    }
    if ((angle_scale > 0) && (m_scale >= 10))
    {
        resetTransform();
        return;
    }
    else if ((angle_scale < 0) && (m_scale <= 0.01))
    {
        return;
    }
    else
    {
        m_scale = angle_scale > 0 ? m_scale * 1.1 : m_scale * 0.9;
        scaleView(angle_scale ? 1.1 : 0.1);
    }

    QGraphicsView::wheelEvent(event);
}

void GraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    m_mouse_current_pos = event->pos();
    if (event->buttons() & Qt::RightButton)
    {
        m_move_view_flag = true;
        QPointF delta = mapToScene(event->pos()) - mapToScene(m_mouse_clike_pos);
        setSceneRect(sceneRect().translated(-delta));
        m_mouse_clike_pos = m_mouse_current_pos;
    }

    if (m_is_drawing)
    {
        m_preview_line.setVisible(true);
        QPointF start(mapToScene(m_mouse_clike_pos));
        QPointF end(mapToScene(m_mouse_current_pos));
        m_preview_line.update_point(start, end);
        m_preview_line.line_color = m_line_color;
    }

    QGraphicsView::mouseMoveEvent(event);
}

void GraphicsView::mousePressEvent(QMouseEvent *event)
{
    m_mouse_clike_pos = event->pos();
    // 禁用右键框选功能
    if (event->button() == Qt::RightButton)
    {
        m_right_button_pressed = true;
    }
    if (event->button() == Qt::LeftButton)
    {
        m_left_button_pressed = true;
        // 尝试获取点击位置窗口信息
        auto info = node_manager.get_port_info(m_mouse_clike_pos);
        if (!info.is_empty())
        {
            m_is_drawing = true;
            m_line_color = info.port->color;
        }
    }

    QGraphicsView::mousePressEvent(event);
}

void GraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    // 不显示画线预览线
    m_preview_line.setVisible(false);
    m_mouse_release_pos = event->pos();

    if (event->button() == Qt::RightButton)
    {
        m_right_button_pressed = false;
    }

    // 左键释放
    if (event->button() == Qt::LeftButton)
    {
        m_left_button_pressed = false;
        // 尝试获取释放位置的端口信息
        auto release_info = node_manager.get_port_info(m_mouse_release_pos);
        auto click_info = node_manager.get_port_info(m_mouse_clike_pos);

        if (!release_info.is_empty() && m_is_drawing)
        {
            auto port_check = node_manager.port_type_check(click_info, release_info);
            auto monotonicity_check = node_manager.port_monotonicity_check(click_info, release_info);
            auto data_type_check = node_manager.port_data_type_check(click_info, release_info);

            if (port_check && data_type_check)
            {
                if (monotonicity_check)
                {
                    node_manager.port_connect(click_info, release_info);
                }
                else
                {
                    node_manager.port_reconnect(click_info, release_info);
                }
            }

            click_info.node->setFlag(QGraphicsItem::ItemIsMovable, true);
        }
        else if (m_is_drawing && release_info.is_empty() && !click_info.is_empty())
        {
            // 移除线
            if (click_info.port->type == Port::Input || click_info.port->type == Port::InputForce)
            {
                node_manager.delete_port_connect(click_info);
            }
        }
    }

    // 重置画线标识
    m_is_drawing = false;
    QGraphicsView::mouseReleaseEvent(event);
}
void GraphicsView::dragMoveEvent(QDragMoveEvent *event)
{
    event->accept();
}
void GraphicsView::dragEnterEvent(QDragEnterEvent *event)
{
    event->accept();
}
void GraphicsView::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
}
void GraphicsView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    auto size = event->size();
    setSceneRect(0, 0, size.width(), size.height());
}

void GraphicsView::dropEvent(QDropEvent *event)
{
    auto label = dynamic_cast<DragLabel *>(event->source());
    auto name = label->text();

    QPointF pos(mapToScene(event->position().toPoint()));

    auto node = node_manager.lib_manager->create_node(name.toStdString(), pos);
    if (node != nullptr)
    {
        node_manager.add_node(node);
        std::cout << name.toStdString() << " Add" << std::endl;
    }
    else
    {
        std::cout << name.toStdString() << " not support!" << std::endl;
    }
}

bool GraphicsView::viewportEvent(QEvent *event)
{
    return QGraphicsView::viewportEvent(event);
}
void GraphicsView::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Control)
    {
        m_scoll_flag = true;
    }
}
void GraphicsView::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_F5)
    {
        // 执行节点
        node_manager.run();
    }
    else if (event->key() == Qt::Key_Delete)
    {
        // 删除选中
        node_manager.delete_selected();
    }
    else if (event->key() == Qt::Key_Control)
    {
        m_scoll_flag = false;
    }
}

void GraphicsView::contextMenuEvent(QContextMenuEvent *event)
{
    event->accept();
}

void GraphicsView::scaleView(qreal scale_factor)
{
    scale(scale_factor, scale_factor);
}
