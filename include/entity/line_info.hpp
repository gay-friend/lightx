#pragma once

#include "nodes/port.h"
#include "widgets/bezier_curve_item.h"

/// @brief 线信息
class LineInfo
{
public:
    Port *port1;                    // 端口1
    Port *port2;                    // 端口2
    BezierCurveItem *line{nullptr}; // 线实例

    LineInfo(Port *port1, Port *port2, BezierCurveItem *line)
        : port1(port1), port2(port2), line(line)
    {
    }
    /// @brief 获取另一个端口
    /// @param port 端口
    /// @return 另一个端口
    Port *get_other_port_info_by_port(Port *port)
    {
        return port1 != port ? port1 : port2;
    }
    void delete_line()
    {
        port1->is_connected = false;
        port2->is_connected = false;
        delete line;
        line = nullptr;
    }
};