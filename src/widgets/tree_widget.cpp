#include "widgets/tree_widget.h"

NodeListWidget::NodeListWidget(QWidget *parent, bool drag_enabled)
{
    setColumnCount(1);
    setHeaderHidden(true);
    setObjectName("MenuTree");

    if (drag_enabled)
    {
        setDragEnabled(true);
        viewport()->setAcceptDrops(false);
        setDragDropMode(QTreeWidget::DragDrop);
        setDropIndicatorShown(true);

        auto root_item = invisibleRootItem();
        root_item->setFlags(root_item->flags() ^ Qt::ItemIsDragEnabled);
    }
}

void NodeListWidget::startDrag(Qt::DropActions supportedActions)
{
    dragged_item = itemAt(mapFromGlobal(QCursor::pos()));
    QTreeWidget::startDrag(supportedActions);
}

void NodeListWidget::build_tree(std::map<std::string, std::map<std::string, func_create_node *>> m_func_map)
{
    clear();
    QList<QTreeWidgetItem *> items;
    for (auto pkg : m_func_map)
    {
        auto pkg_name = QString::fromStdString(pkg.first);
        auto item = new QTreeWidgetItem({pkg_name});
        auto flags = item->flags();
        flags ^= Qt::ItemIsSelectable;
        flags ^= Qt::ItemIsDropEnabled;
        item->setFlags(flags);

        for (auto comm : pkg.second)
        {
            auto node_name = QString::fromStdString(comm.first);
            auto node_item = new QTreeWidgetItem({node_name});
            QList<QString> data{pkg_name, node_name};
            node_item->setData(0, Qt::UserRole, data);
            node_item->setFlags(node_item->flags() ^ Qt::ItemIsDropEnabled);
            item->addChild(node_item);
        }
        items.append(item);
    }

    insertTopLevelItems(0, items);
}
