#pragma once

#include <QMap>
#include <QPainter>
#include <QGraphicsItem>
#include <opencv2/opencv.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Port : public QGraphicsObject
{
    Q_OBJECT
signals:
    void on_value_change(QVariant *value);

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
        File,
        Image,
    };

    Port(const std::string &node_id, uint id, const std::string &name = "Port", Type type = Input, DataType data_type = Int);
    json to_json();
    void load_from_json(json config);
    QRectF boundingRect() const;
    virtual QPointF get_port_pos() const;
    virtual void connect(Port *port);
    virtual void disconnect();
    bool readonly();
    bool is_pair();
    template <typename T>
    void set_value(T value)
    {
        if (m_parent == nullptr)
        {
            m_data->setValue(QVariant::fromValue(value));
            emit on_value_change(m_data);
        }
        else
        {
            m_parent->set_value(value);
        }
    }
    template <typename T>
    T get_value() const
    {
        return m_parent == nullptr ? m_data->value<T>() : m_parent->get_value<T>();
    }
    void set_parent(Port *port);
    void add_child(Port *port);
    void remove_child(Port *port);
    QVariant *get_data();

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
    std::vector<Port *> m_childs;
    QVariant *m_data;
    inline static std::map<DataType, QColor> COLOR_MAP{
        {Port::Float, QColor("#2fFF09")},
        {Port::Int, QColor("#008000")},
        {Port::Bool, QColor("#ff0606")},
        {Port::String, QColor("#be0ba0")},
        {Port::Image, QColor("#00BFFF")},
        {Port::File, QColor("#b92ee6")},
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
    virtual QPointF get_port_pos() const override;
};
