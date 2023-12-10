#pragma once
#include <iostream>

#include "nodes/node.h"
#include "utils/image_utils.h"

#include <QImage>
#include <QList>
#include <QFileDialog>
#include <filesystem>

/// @brief 创建节点
/// @param pos 坐标
/// @return
NODE_API Node *create_node(QPointF pos);
/// @brief 获取节点信息
/// @return
NODE_API NodeInfo get_node_info();

/// @brief 节点信息
NodeInfo NODE_INFO{"CameraNode", Node::CameraNode};

/// @brief 相机节点
class CameraNode : public Node
{
public:
    /// @brief 构造函数
    /// @param pos 坐标
    CameraNode(QPointF pos);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    /// @brief 执行节点
    void execute() override;
    /// @brief 初始化
    void init() override;
    /// @brief 反初始化
    void uninit() override;
    std::string dir{"/home/torchlab/cpp-learn/prod/assets/images"};

    QImage image;

private:
    std::vector<std::string> m_image_files;
    int m_index{0};
};
