#pragma once

#include <QMainWindow>
#include <QDockWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QApplication>
#include <QToolBar>
#include <QSplitter>

#include "widgets/graphics_view.h"
#include "widgets/sidebar_widget.h"
#include "widgets/tree_widget.h"

class EditorWindow : public QMainWindow
{
    Q_OBJECT
public:
    EditorWindow(QWidget *parent = nullptr);

public slots:
    void on_action_run();
    void on_node_select(NodeWidget *w);

private:
    QToolBar *m_tool_bar;
    /// @brief 剪贴板
    QClipboard *m_clipboard;
    GraphicsView *m_editor;
    QSplitter *m_splitter;
    QSplitter *m_center_splitter;
    QWidget *m_left_sidebar;
    QDockWidget *m_right_dock;
    QVBoxLayout *m_left_layout;
    std::string m_selected_node_id{""};
};

int run_ui(int argc, char *argv[]);
