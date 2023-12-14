#pragma once
#include <QTreeWidget>
#include "utils/utils.h"

class NodeListWidget : public QTreeWidget
{
    Q_OBJECT
public:
    NodeListWidget(QWidget *parent = nullptr, bool drag_enabled = false);
    void startDrag(Qt::DropActions supportedActions) override;
    void build_tree(std::map<std::string, std::map<std::string, func_create_node *>> m_func_map);

    QTreeWidgetItem *dragged_item;
};
