#include "widgets/graphics_view.h"

GraphicsView::GraphicsView(QWidget *parent) : QGraphicsView(parent)
{
    // 设置框选模式
    setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing |
                   QPainter::SmoothPixmapTransform | QPainter::LosslessImageRendering);

    // 设置缓冲背景 加速渲染
    // setCacheMode(QGraphicsView::CacheBackground);
    // setMouseTracking(true);                               // 跟踪鼠标位置
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // 隐藏滚动条
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);   // 隐藏滚动条

    // 启用框选功能
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setDragMode(QGraphicsView::RubberBandDrag);
    // 支持拖拽
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
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
    auto zoom_factor = event->angleDelta().y() > 0 ? m_zoom_factor : 1.0 / m_zoom_factor;
    m_view_scale *= zoom_factor;
    if (m_view_scale < 0.2 || m_view_scale > 3)
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

    if (m_is_drawing)
    {
        m_preview_line.setVisible(true);

        m_preview_line.update_point(mapToScene(m_mouse_current_pos));
        m_preview_line.line_color = m_line_color;
    }
    else
    {
        QGraphicsView::mouseMoveEvent(event);
    }
}

void GraphicsView::mousePressEvent(QMouseEvent *event)
{
    m_mouse_clike_pos = event->pos();
    auto port = node_manager.get_port(m_mouse_clike_pos);
    switch (event->button())
    {
    case Qt::LeftButton:
        if (port != nullptr && !m_drag_mode)
        {
            m_is_drawing = true;
            m_line_color = port->color;
            m_preview_line.update_point(port->get_port_pos(), port->get_port_pos());
            return;
        }

        break;
    case Qt::RightButton:
        // 创建虚拟的左键松开事件
        QGraphicsView::mouseReleaseEvent(new QMouseEvent(QEvent::MouseButtonRelease, event->position(), mapToGlobal(event->position()),
                                                         Qt::LeftButton, Qt::NoButton, event->modifiers()));

        setDragMode(QGraphicsView::ScrollHandDrag);
        m_drag_mode = true;
        // 创建虚拟的左键点击事件
        QGraphicsView::mousePressEvent(new QMouseEvent(QEvent::MouseButtonRelease, event->position(), mapToGlobal(event->position()),
                                                       Qt::LeftButton, Qt::NoButton, event->modifiers()));
        break;

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
    Port *release_port;
    Port *click_port;

    switch (event->button())
    {
    case Qt::LeftButton:
        // 尝试获取释放位置的端口信息
        release_port = node_manager.get_port(m_mouse_release_pos);
        click_port = node_manager.get_port(m_mouse_clike_pos);

        if (release_port != nullptr && m_is_drawing)
        {
            if (node_manager.port_type_check(click_port, release_port) &&
                node_manager.port_data_type_check(click_port, release_port))
            {
                if (node_manager.port_monotonicity_check(click_port, release_port))
                {
                    node_manager.port_connect(click_port, release_port);
                }
                else
                {
                    node_manager.port_reconnect(click_port, release_port);
                }
            }
            node_manager.get_node(click_port)->setFlag(QGraphicsItem::ItemIsMovable, true);
        }
        else if (m_is_drawing && release_port == nullptr && click_port != nullptr)
        {
            // 移除线
            if (click_port->type == Port::Input || click_port->type == Port::InputForce)
            {
                node_manager.delete_port_connect(click_port);
            }
        }
        break;
    case Qt::RightButton:
        // 创建虚拟的左键松开事件
        QGraphicsView::mouseReleaseEvent(new QMouseEvent(QEvent::MouseButtonRelease, event->position(), mapToGlobal(event->position()),
                                                         Qt::LeftButton, Qt::NoButton, event->modifiers()));
        setDragMode(QGraphicsView::RubberBandDrag);
        m_drag_mode = false;
        break;

    default:
        break;
    }

    // 重置画线标识
    m_is_drawing = false;
    QGraphicsView::mouseReleaseEvent(event);
}
void GraphicsView::dragMoveEvent(QDragMoveEvent *event)
{
    try
    {
        dynamic_cast<NodeListWidget *>(event->source());
        event->acceptProposedAction();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        QGraphicsView::dragMoveEvent(event);
    }
}

void GraphicsView::dropEvent(QDropEvent *event)
{
    try
    {
        auto widget = dynamic_cast<NodeListWidget *>(event->source());
        auto item = widget->dragged_item;
        if (item == nullptr)
        {
            std::cerr << "empty drop item" << std::endl;
            return;
        }
        auto data = item->data(0, Qt::UserRole).value<QList<QString>>();
        QPointF pos(mapToScene(event->position().toPoint()));

        auto node = node_manager.lib_manager->create_node(data[0].toStdString(), data[1].toStdString());
        auto node_widget = new NodeWidget(node, pos);

        if (node != nullptr)
        {
            node_manager.add_node(node_widget);
            std::cout << data[1].toStdString() << " Add" << std::endl;
        }
        else
        {
            std::cout << data[1].toStdString() << " not support!" << std::endl;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        QGraphicsView::dropEvent(event);
    }
}

void GraphicsView::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_F5:
        // 执行节点
        node_manager.run();
        break;
    case Qt::Key_Delete:
        // 删除选中
        node_manager.delete_selected();
    default:
        break;
    }
}

void GraphicsView::contextMenuEvent(QContextMenuEvent *event)
{
    event->accept();
}
