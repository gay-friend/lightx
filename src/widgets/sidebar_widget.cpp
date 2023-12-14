#include <widgets/sidebar_widget.h>

SidebarWidget::SidebarWidget(QWidget *parent, const std::string &title, bool is_stretch) : title(title), is_stretch(is_stretch), QWidget(parent)
{
    auto v_layout = new QVBoxLayout(this);
    v_layout->setContentsMargins(0, 0, 0, 0);

    auto title_widget = new QWidget(this);
    title_widget->setObjectName("SideBarTitle");
    auto title_layout = new QHBoxLayout(title_widget);
    title_layout->setContentsMargins(20, 0, 20, 0);

    auto title_label = new QLabel(QString::fromStdString(title));
    auto menu_button = new QPushButton("...");

    title_layout->addWidget(title_label);
    title_layout->addStretch(1);
    title_layout->addWidget(menu_button);

    m_content_widget = new ToolBoxWidget(this, is_stretch);

    v_layout->addWidget(title_widget);
    v_layout->addWidget(m_content_widget);
    v_layout->setSpacing(0);

    setAttribute(Qt::WA_StyledBackground);
}

void SidebarWidget::add_comp(const std::string &comp_name, NodeListWidget *widget, bool collapsed, bool stretch)
{
    auto comp = m_content_widget->add_component(comp_name, widget, collapsed, is_stretch);
}

ToolBoxWidget::ToolBoxWidget(QWidget *parent, bool is_stretch) : QWidget(parent), is_stretch(is_stretch)
{

    m_container = new QWidget(this);
    m_container_layout = new QVBoxLayout(m_container);
    m_container_layout->setContentsMargins(0, 0, 0, 0);
    m_container_layout->setSpacing(0);
    m_container->setObjectName("container");

    m_spacer = new QSpacerItem(width(), height(), QSizePolicy::Maximum, QSizePolicy::Expanding);

    m_widget = new QWidget(this);
    m_v_layout = new QVBoxLayout(m_widget);
    m_v_layout->setContentsMargins(0, 0, 0, 0);
    m_v_layout->addWidget(m_container);

    m_scroll_area = new QScrollArea(this);
    m_out_layout = new QVBoxLayout(this);
    m_out_layout->addWidget(m_scroll_area);
    m_out_layout->setContentsMargins(0, 0, 0, 0);

    m_scroll_area->setWidget(m_widget);
    // 这句话非常重要 要不然会不显示
    m_scroll_area->setWidgetResizable(true);
    m_scroll_area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setObjectName("ToolBox");
}

ToolBoxComponentWidget *ToolBoxWidget::add_component(const std::string &title, NodeListWidget *widget, bool collapsed, int stretch)
{
    auto component = new ToolBoxComponentWidget(this, collapsed, stretch);
    component->setup_widget(title, widget);
    m_container_layout->addWidget(component);

    m_components.push_back(component);
    return component;
}

ToolBoxComponentWidget::ToolBoxComponentWidget(QWidget *parent, bool collapsed, int default_stretch)
    : QWidget(parent), m_collapsed(collapsed), m_default_stretch(default_stretch)
{
}

void ToolBoxComponentWidget::setup_widget(const std::string &title, QWidget *content)
{
    auto v_layout = new QVBoxLayout(this);
    v_layout->setContentsMargins(0, 0, 0, 0);

    m_title_widget = new QPushButton(this);
    m_title_widget->setObjectName("ToolCompTitle");
    auto title_layout = new QHBoxLayout(m_title_widget);
    title_layout->setContentsMargins(10, 0, 0, 0);

    auto icon_label = new QLabel(m_title_widget);
    icon_label->setFixedSize(12, 12);
    icon_label->setObjectName("labelIcon");
    title_layout->addWidget(icon_label);
    title_layout->addWidget(new QLabel(QString::fromStdString(title), m_title_widget));
    title_layout->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

    v_layout->addWidget(m_title_widget);
    m_content_widget = content;
    m_content_widget->setObjectName("toolContent");
    v_layout->addWidget(m_content_widget);
    v_layout->addSpacing(0);

    if (m_collapsed)
    {
        m_content_widget->hide();
    }
    else
    {
        m_content_widget->show();
    }
}