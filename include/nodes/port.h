#pragma once

#include <QMap>
#include <QPainter>
#include <QGraphicsItem>
#include <opencv2/opencv.hpp>

class Port : public QGraphicsItem
{
public:
    enum Type
    {
        Input,      // 输入
        InputForce, // 强制输入
        Output      // 输出
    };
    enum DataType
    {
        Float,
        Int,
        Bool,
        String,
        Image,
    };

    Port(const std::string &node_id, uint id, const std::string &name = "Port", Type type = Input, DataType data_type = Int);
    QRectF boundingRect() const;
    virtual QPointF get_port_pos();
    virtual void connect(Port *port);
    virtual void disconnect();
    template <typename T>
    void set_value(T value)
    {
        m_parent == nullptr ? m_data->setValue(QVariant::fromValue(value)) : m_parent->set_value(value);
    }
    template <typename T>
    T get_value()
    {
        return m_parent == nullptr ? m_data->value<T>() : m_parent->get_value<T>();
    }
    void set_parent(Port *port);

    std::string node_id;
    uint id;
    std::string name;
    Type type;
    DataType data_type;
    QColor color{"#ffffff"};
    bool is_connected{false};
    int icon_size{15};
    int port_width;

protected:
    Port *m_parent{nullptr};
    QVariant *m_data;
    inline static std::map<DataType, QColor> COLOR_MAP{
        {Port::Float, QColor("#2fFF09")},
        {Port::Int, QColor("#2fFF09")},
        {Port::Bool, QColor("#2fFF09")},
        {Port::String, QColor("#2fFF09")},
        {Port::Image, QColor("#00BFFF")},
    };
    QPen m_pen_default;
    QBrush m_brush_default;
    QFont m_font;
    int m_font_size{14};
    int m_label_size;
};

class InputPort : public Port
{
public:
    InputPort(const std::string &node_id, uint id, const std::string &name = "Port", Type type = Input, DataType data_type = Int);
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

class OutputPort : public Port
{
public:
    OutputPort(const std::string &node_id, uint id, const std::string &name = "Port", Type type = Input, DataType data_type = Int);
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    virtual QPointF get_port_pos() override;
};
