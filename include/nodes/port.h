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
        Int,
        Bool,
        String,
        Float,
        Image,
    };

    Port(uint id, const std::string &name = "Port", Type type = Input, DataType data_type = Int, QColor color = "#ffffff");
    QRectF boundingRect() const;

    uint id;
    std::string name;
    Type type;
    DataType data_type;
    QColor color{"#ffffff"};
    QVariant data;
    bool is_connected{false};
    int icon_size{15};
    int port_width;

protected:
    QPen m_pen_default;
    QBrush m_brush_default;
    QFont m_font;
    int m_font_size{14};
    int m_label_size;
};

class InputPort : public Port
{
public:
    InputPort(uint id, const std::string &name = "Port", Type type = Input, DataType data_type = Int, QColor color = "#ffffff");

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

class OutputPort : public Port
{
public:
    OutputPort(uint id, const std::string &name = "Port", Type type = Input, DataType data_type = Int, QColor color = "#ffffff");
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};
