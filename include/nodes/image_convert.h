#pragma once

#include "nodes/node.hpp"
#include "utils/image_utils.hpp"
#include <QImage>
#include <iostream>

/// @brief 创建节点
/// @param pos 坐标
/// @return
NODE_API NodeWidget *create_node(QPointF pos);
/// @brief 获取节点信息
/// @return
NODE_API NodeInfo get_node_info();

/// @brief 节点信息
NodeInfo NODE_INFO{"ImageConvertNode", NodeWidget::CameraNode};

class ImageConvertNode : public NodeWidget
{
public:
    /// @brief 构造函数
    /// @param pos 坐标
    ImageConvertNode(QPointF pos);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    /// @brief 执行节点
    void execute() override;
    /// @brief 初始化
    void init() override;
    /// @brief 反初始化
    void uninit() override;

    QImage image;
};