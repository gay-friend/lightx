#include "nodes/port.h"

Port::Port() {}

Port::~Port() {}

Port::Port(uint id, const std::string &name, Type type, DataType data_type, QVariant data)
    : type(type), id(id), name(name), data(data), data_type(data_type), color(PORT_COLOR_MAP[data_type])
{
    // 设置端口颜色范围和端口文字范围和文字框对齐方向
    rect = (type == Input || type == InputForce) ? QRectF(5, 50 + id * 42, 20, 20) : QRectF(135, 50 + id * 42, 20, 20);
    text_rect = (type == Input || type == InputForce) ? QRectF(28, 50 + id * 42, 50, 28) : QRectF(83, 50 + id * 42, 50, 28);
    text_align = (type == Input || type == InputForce) ? Qt::AlignLeft : Qt::AlignRight;
    name_text_rect = (type == Input || type == InputForce) ? QRectF(5, rect.y() - 20, 50, 20) : QRectF(105, rect.y() - 20, 50, 20);
}

void Port::update()
{
    // 设置端口颜色范围和端口文字范围和文字对齐方向
    rect = (type == Input || type == InputForce) ? QRectF(3, 45 + id * 30, 20, 20) : QRectF(127, 45 + id * 30, 20, 20);
    text_rect = (type == Input || type == InputForce) ? QRectF(25, 45 + id * 28, 50, 28) : QRectF(75, 45 + id * 28, 50, 28);
    text_align = (type == Input || type == InputForce) ? Qt::AlignLeft : Qt::AlignRight;

    color = PORT_COLOR_MAP[data_type];
}

bool Port::port_type_check(Type traporttype)
{
    switch (traporttype)
    {
    case Input:
    case InputForce:
        return type == Output;
    case Output:
        return type == Input;
    default:
        return false;
    }
}
bool Port::set_port_value(Port *port)
{
    // TODO: 自动数据转换
    if (port->data_type == this->data_type)
    {
        this->data = port->data;
        return true;
    }
    return false;
}

template <typename T>
void Port::set_value(T value)
{
    // TODO: 自动数据转换
    if (typeid(value) == typeid(cv::Mat))
    {
        data.setValue(QVariant::fromValue(value));
    }
    else
    {
        data.setValue(value);
    }
}
template <typename T>
T Port::get_value()
{
    return data.value<T>();
}
