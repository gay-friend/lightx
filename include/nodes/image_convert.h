#pragma once

#include "nodes/node.h"
#include "utils/image_utils.hpp"
#include <QImage>
#include <iostream>

/// @brief 创建节点
/// @return
NODE_API Node *create_node();
/// @brief 获取节点信息
/// @return
NODE_API NodeInfo get_node_info();

/// @brief 节点信息
NodeInfo NODE_INFO{"Convert", Node::CameraNode};

class ImageConvertNode : public Node
{
public:
    /// @brief 构造函数
    /// @param pos 坐标
    ImageConvertNode(const std::string &node_name, Type node_type);
    /// @brief 执行节点
    virtual void execute() override;
    /// @brief 初始化
    virtual void init() override;
    /// @brief 反初始化
    virtual void uninit() override;

    QImage image;
};