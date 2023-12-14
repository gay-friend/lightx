#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include "widgets/tree_widget.h"

class ToolBoxComponentWidget : public QWidget
{
    Q_OBJECT
public:
    ToolBoxComponentWidget(QWidget *parent = nullptr, bool collapsed = true, int default_stretch = 10);
    void setup_widget(const std::string &title, QWidget *content);

private:
    QWidget *m_title_widget;
    QWidget *m_content_widget;
    int m_default_stretch;
    bool m_collapsed;
};

class ToolBoxWidget : public QWidget
{
    Q_OBJECT
public:
    ToolBoxWidget(QWidget *parent = nullptr, bool is_stretch = false);
    ToolBoxComponentWidget *add_component(const std::string &title, NodeListWidget *widget, bool collapsed = true, int stretch = 10);

    bool is_stretch;

private:
    QScrollArea *m_scroll_area;
    QVBoxLayout *m_out_layout;
    QVBoxLayout *m_v_layout;
    QVBoxLayout *m_container_layout;
    QWidget *m_container;
    QWidget *m_widget;
    QSpacerItem *m_spacer;
    std::vector<ToolBoxComponentWidget *> m_components;
};

class SidebarWidget : public QWidget
{
public:
    SidebarWidget(QWidget *parent = nullptr, const std::string &title = "", bool is_stretch = false);
    void add_comp(const std::string &comp_name, NodeListWidget *widget, bool collapsed = true, bool stretch = 10);

    std::string title;
    bool is_stretch;

private:
    ToolBoxWidget *m_content_widget;
};