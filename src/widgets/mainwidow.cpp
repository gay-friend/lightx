#include "widgets/mainwidow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setMinimumSize(960, 800);
    m_left_dock = new QDockWidget("", this);
    m_right_dock = new QDockWidget("信息", this);

    // 禁用关闭按钮
    m_left_dock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    m_left_dock->setMinimumWidth(150);
    m_right_dock->setFeatures(QDockWidget::NoDockWidgetFeatures);

    m_view = new GraphicsView(this);
    setCentralWidget(m_view);

    addDockWidget(Qt::LeftDockWidgetArea, m_left_dock);
    addDockWidget(Qt::RightDockWidgetArea, m_right_dock);

    auto widget = new QWidget();
    auto layout = new QVBoxLayout();
    for (auto it : m_view->node_manager.get_all_node_names())
    {
        layout->addWidget(new DragLabel(it));
    }

    layout->addStretch();
    widget->setLayout(layout);
    m_left_dock->setWidget(widget);

    // 设置工具栏
    m_tool_bar = new QToolBar(this);
    this->addToolBar(m_tool_bar);

    // 把动作添加到工具栏，QAction就会自动变成工具
    auto run_action = new QAction("Run");
    connect(run_action, SIGNAL(triggered()), this, SLOT(on_action_run()));

    m_tool_bar->addAction(run_action);
}

void MainWindow::on_action_run()
{
    m_view->node_manager.run();
}