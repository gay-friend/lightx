#pragma once

#include <string>
#include <map>
#include <filesystem>
#include <vector>
#include <iostream>
#include <QLibrary>
#include <QDebug>
#include "node/common.h"

/// @brief 加载动态库
/// @param file 库路径
/// @param func_name 导出函数名
/// @param handle 库句柄
/// @param info 节点信息
/// @return

/// @brief 动态库加载管理
class LibManager
{
public:
    /// @brief 构造函数
    /// @param dir 动态库路径
    /// @param func_name 导出函数名
    LibManager(const std::string &dir);
    /// @brief 析构函数
    virtual ~LibManager();
    /// @brief 加载动态库
    void load();
    /// @brief 卸载动态库
    void unload();
    /// @brief 创建节点
    /// @param node_name 节点名
    /// @param pos 坐标
    /// @return 节点
    Node *create_node(const std::string &node_type, const std::string &node_name);
    /// @brief 创建节点函数字典 {节点类型:{节点名: func}}
    std::map<std::string, std::map<std::string, func_create_node>> func_map;

private:
    /// @brief 动态库路径
    std::string m_dir;
    /// @brief 导出函数名
    std::string m_func_name;
    // @brief 动态库句柄字典
    // std::vector<QLibrary> m_libs;
};