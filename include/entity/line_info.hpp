#pragma once

#include "nodes/port.h"
#include "widgets/bezier_curve_item.h"

/// @brief 线信息
class LineInfo
{
public:
    Port *in_port;                  // 端口1
    Port *out_port;                 // 端口2
    BezierCurveItem *line{nullptr}; // 线实例

    ~LineInfo()
    {
    }
    void clear()
    {
        in_port->disconnect();
        if (line != nullptr)
        {
            delete line;
            line = nullptr;
        }
        in_port = nullptr;
        out_port = nullptr;
    }
    LineInfo(Port *port1, Port *port2, BezierCurveItem *line) : line(line)
    {
        if (port1->type == Port::Output)
        {
            in_port = port2;
            out_port = port1;
        }
        else
        {
            in_port = port1;
            out_port = port2;
        }
    }
    bool is_line_port(Port *port) const
    {
        return port == in_port || port == out_port;
    }
    /// @brief 获取另一个端口
    /// @param port 端口
    /// @return 另一个端口
    Port *get_another_port(Port *port) const
    {
        return in_port != port ? in_port : out_port;
    }
    bool operator==(const LineInfo &lineinfo) const
    {
        return this->in_port == lineinfo.in_port && this->out_port == lineinfo.out_port;
    }
};