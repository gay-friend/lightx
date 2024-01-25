#include "widgets/editor.h"

EditorWindow::EditorWindow(QWidget *parent) : QMainWindow(parent)
{
    setMinimumSize(960, 800);
    // 居中显示
    auto screen = QGuiApplication::primaryScreen()->geometry();
    auto size = geometry();
    auto center_x = (screen.width() - size.width()) / 2 - 200;
    auto center_y = (screen.height() - size.height()) / 2;
    move(center_x, center_y);

    // 编辑器设置，位置在中间，占比例至少80 %
    auto manager = new NodeManager();
    m_editor = new GraphicsView(this, manager);
    manager->load_workspace("workspace.json");
    QObject::connect(m_editor, &GraphicsView::on_select, this, &EditorWindow::on_node_select);
    // 初始化菜单栏

    // 初始化侧边栏
    m_left_sidebar = new QWidget(this);
    m_left_layout = new QVBoxLayout(m_left_sidebar);
    m_left_layout->setContentsMargins(0, 0, 0, 0);
    auto left_bar = new SidebarWidget(nullptr, "", false);
    auto model_tree = new NodeListWidget(this, true);
    model_tree->build_tree(m_editor->main_thread->lib_manager->func_map);
    left_bar->add_comp("模块库", model_tree, false, true);
    m_left_layout->addWidget(left_bar);

    // 设置布局的初始大小
    // 设置中间splitter
    m_center_splitter = new QSplitter(Qt::Vertical, this);
    m_center_splitter->addWidget(m_editor);
    m_center_splitter->setSizes({800, 200});

    m_right_dock = new QDockWidget();
    // 禁用关闭按钮
    m_right_dock->setFeatures(QDockWidget::NoDockWidgetFeatures);

    m_splitter = new QSplitter(Qt::Horizontal, this);
    m_splitter->addWidget(m_left_sidebar);
    m_splitter->addWidget(m_center_splitter);
    m_splitter->addWidget(m_right_dock);
    m_splitter->setSizes({250, 1500, 250});
    setCentralWidget(m_splitter);

    // 剪贴板
    m_clipboard = QApplication::clipboard();

    // 设置工具栏
    m_tool_bar = new QToolBar(this);
    this->addToolBar(m_tool_bar);

    // 把动作添加到工具栏，QAction就会自动变成工具
    auto run_action = new QAction("Run");
    connect(run_action, SIGNAL(triggered()), this, SLOT(on_action_run()));

    m_tool_bar->addAction(run_action);
}

void EditorWindow::on_action_run()
{
    m_editor->main_thread->start();
}

void EditorWindow::on_node_select(NodeWidget *w)
{
    if (w->node->uuid == this->m_selected_node_id)
    {
        return;
    }
    m_right_dock->setWidget(w->node);
}

int run_ui(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("LightX");
    app.setApplicationDisplayName("LightX");
    auto window = EditorWindow();
    window.show();
    return app.exec();
}
