#pragma once

#include <string>
#include <map>
#include <filesystem>
#include <vector>
#include <iostream>
#if __WIN32__
#include <libloaderapi.h>
#else
#include <dlfcn.h>
#endif

#include "nodes/node.h"

/// @brief 加载动态库
/// @param file 库路径
/// @param func_name 导出函数名
/// @param handle 库句柄
/// @param info 节点信息
/// @return
func_create_node *load_lib(const char *file, const char *func_name, void **handle, NodeInfo *info);

/// @brief 动态库加载管理
class LibManager
{
public:
    /// @brief 构造函数
    /// @param dir 动态库路径
    /// @param func_name 导出函数名
    LibManager(const std::string &dir, const std::string func_name);
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
    Node *create_node(const std::string &node_name, QPointF pos);
    /// @brief 节点名
    std::vector<std::string> names;

private:
    /// @brief 动态库路径
    std::string m_dir;
    /// @brief 导出函数名
    std::string m_func_name;
    /// @brief 动态库句柄字典
    std::map<std::string, void *> m_handle_map;
    /// @brief 创建节点函数字典
    std::map<std::string, func_create_node *> m_func_map;
};