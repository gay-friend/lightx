#pragma once

#include <QMainWindow>
#include <QDockWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QApplication>
#include <QToolBar>

#include "widgets/graphics_view.h"
#include "widgets/drag_label.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);

    QUrl ProjectUrl;      // 项目位置
    GraphicsView *m_view; // 中间编辑视图窗口

public slots:
    void on_action_run();

private:
    QDockWidget *m_right_dock;
    QDockWidget *m_left_dock;
    QToolBar *m_tool_bar;
};
