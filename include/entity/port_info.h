#pragma once

#include "nodes/node.h"
#include "nodes/port.h"

/// @brief 端口信息
class PortInfo
{
public:
    /// @brief 所属节点
    Node *node{nullptr};
    /// @brief 端口
    Port *port{nullptr};
    /// @brief 是否空节点
    /// @return bool
    bool is_empty()
    {
        return node == nullptr || port == nullptr;
    }
};