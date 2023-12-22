#pragma once

#include "nodes/node.h"
#include "nodes/port.h"

/// @brief 端口信息
class PortInfo
{
public:
    /// @brief 所属节点
    NodeWidget *node_widget{nullptr};
    /// @brief 端口
    Port *port{nullptr};
    /// @brief 是否空节点
    /// @return bool
    bool is_empty()
    {
        return node_widget == nullptr || port == nullptr;
    }
};