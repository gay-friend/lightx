#pragma once

#include "port_info.h"

/// @brief 线信息
class LineInfo
{
public:
    PortInfo port_info1;            // 端口1
    PortInfo port_info2;            // 端口2
    BezierCurveItem *line{nullptr}; // 线实例

    /// @brief 构造函数
    LineInfo() {}
    /// @brief 构造函数
    /// @param port_info1 端口1
    /// @param port_info2 端口2
    /// @param line 线
    LineInfo(PortInfo port_info1, PortInfo port_info2, BezierCurveItem *line)
        : port_info1(port_info1), port_info2(port_info2)
    {
    }
    /// @brief 获取另一个端口
    /// @param port 端口
    /// @return 另一个端口
    PortInfo get_other_port_info_by_port(Port *port)
    {
        return port_info1.port != port ? port_info1 : port_info2;
    }
    void delete_line()
    {
        port_info1.port->is_connected = false;
        port_info2.port->is_connected = false;
        delete line;
        line = nullptr;
    }
};