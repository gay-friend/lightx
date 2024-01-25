#include "widgets/graphics_view.h"

GraphicsView::GraphicsView(QWidget *parent, NodeManager *manager) : QGraphicsView(parent), main_thread(manager)
{
    // 设置框选模式
    setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing |
                   QPainter::SmoothPixmapTransform | QPainter::LosslessImageRendering);
    // 设置缓冲背景 加速渲染
    setCacheMode(QGraphicsView::CacheNone);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // 隐藏水平滚动条
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);   // 隐藏垂直滚动条

    // 启用框选功能
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setDragMode(QGraphicsView::RubberBandDrag);
    // 支持拖拽
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setAcceptDrops(true);

    // 创建场景
    m_scene = new Scene();
    setScene(m_scene);
    // 添加连续预览线到场景
    m_scene->addItem(&m_preview_line);
    m_preview_line.setVisible(false);

    QObject::connect(manager, &NodeManager::on_node_add, this, &GraphicsView::add_node);
    QObject::connect(manager, &NodeManager::on_line_add, this, &GraphicsView::add_line);
    QObject::connect(manager, &NodeManager::on_scene_update, [this]()
                     { this->scene()->update(); });
}
void GraphicsView::update_selected_node()
{
    for (auto item : this->m_scene->selectedItems())
    {
        main_thread->update_node(dynamic_cast<NodeWidget *>(item));
    }
}
Port *GraphicsView::get_port(QPoint pos)
{
    return dynamic_cast<Port *>(itemAt(pos));
}
NodeWidget *GraphicsView::get_node(QPoint pos)
{
    auto pos_item = itemAt(pos);
    if (pos_item == nullptr)
    {
        return nullptr;
    }
    auto node_widget = dynamic_cast<NodeWidget *>(pos_item);

    if (node_widget != nullptr)
    {
        return node_widget;
    }
    for (auto item : this->m_scene->selectedItems())
    {
        node_widget = dynamic_cast<NodeWidget *>(item);
        if (node_widget != nullptr && node_widget->isSelected() && node_widget->contains(pos_item->boundingRect().center()))
        {
            return node_widget;
        }
    }
    return nullptr;
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
    auto port = this->get_port(m_mouse_clike_pos);
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
    auto release_port = this->get_port(event->pos());
    auto click_port = this->get_port(m_mouse_clike_pos);
    auto node = this->get_node(event->pos());

    switch (event->button())
    {
    case Qt::LeftButton:
        // 尝试获取释放位置的端口信息

        if (release_port != nullptr && m_is_drawing)
        {
            // port connect
            if (main_thread->port_type_check(click_port, release_port) &&
                main_thread->port_data_type_check(click_port, release_port))
            {
                if (main_thread->port_monotonicity_check(click_port, release_port))
                {
                    main_thread->port_connect(click_port, release_port);
                }
                else
                {
                    main_thread->port_reconnect(click_port, release_port);
                }
            }
            main_thread->get_node(click_port)->setFlag(QGraphicsItem::ItemIsMovable, true);
        }
        else if (m_is_drawing && release_port == nullptr && click_port != nullptr)
        {
            // delete connect
            if (click_port->type == Port::Input || click_port->type == Port::InputForce)
            {
                main_thread->delete_port_connect(click_port);
            }
        }
        else
        {
            if (release_port == nullptr && node != nullptr)
            {
                emit this->on_select(node);
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
void GraphicsView::add_line(BezierCurveItem *line)
{
    this->scene()->addItem(line);
}
void GraphicsView::add_node(NodeWidget *node_widget)
{
    if (node_widget != nullptr)
    {
        this->scene()->addItem(node_widget);
        QObject::connect(node_widget, &NodeWidget::on_change, this, &GraphicsView::update_selected_node);
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
        this->main_thread->create_node(data[0].toStdString(), data[1].toStdString(), pos);
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
        // main_thread->run_once();
        main_thread->start();
        break;
    case Qt::Key_Delete:
        // 删除选中
        main_thread->delete_selected(this->scene()->selectedItems());
    default:
        break;
    }
}

void GraphicsView::contextMenuEvent(QContextMenuEvent *event)
{
    event->accept();
}
