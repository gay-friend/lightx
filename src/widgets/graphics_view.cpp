#include "widgets/graphics_view.h"

GraphicsView::GraphicsView(QWidget *parent) : QGraphicsView(parent)
{
    // 设置框选模式
    setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing |
                   QPainter::SmoothPixmapTransform | QPainter::LosslessImageRendering);

    // 设置缓冲背景 加速渲染
    // setCacheMode(QGraphicsView::CacheBackground);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setMouseTracking(true);                               // 跟踪鼠标位置
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // 隐藏滚动条
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);   // 隐藏滚动条

    // 启用框选功能
    setDragMode(QGraphicsView::RubberBandDrag);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    // 支持拖拽
    setAcceptDrops(true);

    auto *scene = new Scene();
    setScene(scene);
    // 添加连续预览线到场景
    scene->addItem(&m_preview_line);
    m_preview_line.setVisible(false);
}

void GraphicsView::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
    {
        resetTransform();
        m_view_scale = 1.0;
    }
    QGraphicsView::mouseDoubleClickEvent(event);
}
void GraphicsView::wheelEvent(QWheelEvent *event)
{
    if (m_is_drawing || m_drag_mode)
    {
        return;
    }
    auto zoom_factor = event->angleDelta().y() > 0 ? m_zoom_factor : 1 / m_zoom_factor;
    m_view_scale *= zoom_factor;
    if (m_view_scale < 0.2 || m_view_scale > 2)
    {
        zoom_factor = 1.0;
        m_view_scale = m_last_scale;
    }
    m_last_scale = m_view_scale;
    // 每一次相对于上一次的进行缩放
    scale(zoom_factor, zoom_factor);
}
void GraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    m_mouse_current_pos = event->pos();
    if (event->buttons() & Qt::RightButton)
    {
        QPointF delta = mapToScene(m_mouse_current_pos) - mapToScene(m_mouse_clike_pos);
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
    PortInfo info = node_manager.get_port_info(m_mouse_clike_pos);
    switch (event->button())
    {
    case Qt::LeftButton:
        if (!info.is_empty())
        {
            m_is_drawing = true;
            m_line_color = info.port->color;
        }
        break;
    case Qt::RightButton:
        if (info.is_empty() && event->modifiers() == Qt::ControlModifier)
        {
            QApplication::setOverrideCursor(Qt::CrossCursor);
        }
        setDragMode(QGraphicsView::NoDrag);
        m_drag_mode = true;
        break;
        // case Qt::MiddleButton:
        //     if (!info.is_empty())
        //     {
        //         break;
        //     }
        //     // 创建虚拟的左键松开事件
        //     QGraphicsView::mouseReleaseEvent(new QMouseEvent(QEvent::MouseButtonPress, event->pos(),
        //                                                      Qt::LeftButton, Qt::NoButton, event->modifiers()));
        //     setDragMode(QGraphicsView::ScrollHandDrag);
        //     m_draw_mode = true;
        //     // 创建虚拟的左键点击事件
        //     QGraphicsView::mousePressEvent(new QMouseEvent(QEvent::MouseButtonPress, event->pos(),
        //                                                    Qt::LeftButton, Qt::NoButton, event->modifiers()));
        //     break;

    default:
        break;
    }
    QGraphicsView::mousePressEvent(event);
}

void GraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    // 不显示画线预览线
    m_preview_line.setVisible(false);
    m_mouse_release_pos = event->pos();
    PortInfo release_info, click_info;

    switch (event->button())
    {
    case Qt::LeftButton:
        // 尝试获取释放位置的端口信息
        release_info = node_manager.get_port_info(m_mouse_release_pos);
        click_info = node_manager.get_port_info(m_mouse_clike_pos);

        if (!release_info.is_empty() && m_is_drawing)
        {
            if (node_manager.port_type_check(click_info, release_info) &&
                node_manager.port_data_type_check(click_info, release_info))
            {
                if (node_manager.port_monotonicity_check(click_info, release_info))
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
        break;
    case Qt::RightButton:
        QApplication::setOverrideCursor(Qt::ArrowCursor);
        setDragMode(QGraphicsView::RubberBandDrag);
        m_drag_mode = false;
        break;
        // case Qt::MiddleButton:
        //     QGraphicsView::mouseReleaseEvent(new QMouseEvent(QEvent::MouseButtonPress, event->pos(),
        //                                                      Qt::LeftButton, Qt::NoButton, event->modifiers()));
        //     setDragMode(QGraphicsView::RubberBandDrag);
        //     m_draw_mode = false;
        //     break;

    default:
        break;
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
    QGraphicsView::keyPressEvent(event);
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
}

void GraphicsView::contextMenuEvent(QContextMenuEvent *event)
{
    event->accept();
}
