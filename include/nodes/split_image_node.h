#pragma once

#include "nodes/node.h"
#include <QImage>
#include <iostream>

/// @brief 创建节点
/// @param pos 坐标
/// @return
NODE_API Node *create_node(QPointF pos);
/// @brief 获取节点信息
/// @return
NODE_API NodeInfo get_node_info();

/// @brief 节点信息
NodeInfo NODE_INFO{"SplitImageNode", Node::CameraNode};

class SplitImageNode : public Node
{
public:
    /// @brief 构造函数
    /// @param pos 坐标
    SplitImageNode(QPointF pos);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    /// @brief 执行节点
    void execute() override;
    /// @brief 初始化
    void init() override;
    /// @brief 反初始化
    void uninit() override;

    QImage image;
};