#pragma once

#include <QMap>
#include <QPainter>
#include <QGraphicsItem>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QDoubleValidator>
#include <QPushButton>
#include <QCheckBox>
#include <QComboBox>
#include <nlohmann/json.hpp>

#include "utils/image_utils.hpp"
#include "utils/uuid.hpp"

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
        Enum
    };

    Port(const std::string &node_id, uint index,
         const std::string &name, Type type,
         QColor color);
    virtual void loads(json config);
    virtual json dumps();
    virtual void apply_ui(QVariant *data) = 0;
    virtual void apply_backend() = 0;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QRectF boundingRect() const override;
    QPointF get_port_pos() const;
    void connect(Port *port);
    void disconnect();
    bool readonly();
    bool is_pair();
    template <typename T>
    void set_value(T value)
    {
        if (m_parent == nullptr)
        {
            m_data->setValue(QVariant::fromValue(value));
            // this->apply_ui(m_data);
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
    QLayout *setting_layout{nullptr};

    std::string node_id{""};
    uint index{0};
    std::string uuid{""};
    std::string name{"Node"};
    Type type{Port::Input};
    DataType data_type{Port::String};
    QColor color{"#ffffff"};
    bool is_connected{false};
    int icon_size{15};
    int port_width;
    std::vector<std::string> combo_items;

protected:
    Port *m_parent{nullptr};
    std::vector<Port *> m_childs;
    QVariant *m_data;
    QPen m_pen_default;
    QBrush m_brush_default;
    QFont m_font;
    int m_font_size{14};
    int m_label_size;
};

class StringPort : public Port
{
public:
    StringPort(const std::string &node_id, uint index, const std::string &name, Type type = Input, QColor color = "#be0ba0");
    void loads(json config);
    virtual json dumps() override;
    virtual void apply_ui(QVariant *data);
    virtual void apply_backend() override;

protected:
    QLineEdit *m_edit{nullptr};
};
class FilePort : public StringPort
{
public:
    FilePort(const std::string &node_id, uint index, const std::string &name, Type type = Input, QColor color = "#b92ee6");
};
class IntPort : public StringPort
{
public:
    IntPort(const std::string &node_id, uint index, const std::string &name, Type type = Input, QColor color = "#008000");
    void loads(json config) override;
    virtual void apply_backend() override;
    virtual json dumps() override;
};
class FloatPort : public StringPort
{
public:
    FloatPort(const std::string &node_id, uint index, const std::string &name, Type type = Input, QColor color = "#2fFF09");
    void loads(json config) override;
    virtual void apply_backend() override;
    virtual json dumps() override;
};

class BoolPort : public Port
{
public:
    BoolPort(const std::string &node_id, uint index, const std::string &name, Type type = Input, QColor color = "#ff0606");
    void loads(json config) override;
    virtual json dumps() override;
    virtual void apply_ui(QVariant *data);
    virtual void apply_backend() override;

protected:
    QCheckBox *m_check{nullptr};
};

class EnumPort : public Port
{
public:
    EnumPort(const std::string &node_id,
             uint index, const std::string &name,
             Type type,
             std::vector<std::string> items,
             QColor color = "#a92ee6");
    void loads(json config) override;
    virtual json dumps() override;
    virtual void apply_ui(QVariant *data);
    virtual void apply_backend() override;

protected:
    std::vector<std::string> m_combo_items;
    QComboBox *m_combo{nullptr};
};

class ImagePort : public Port
{
public:
    ImagePort(const std::string &node_id, uint index, const std::string &name, Type type = Input, QColor color = "#00BFFF");
    virtual void apply_ui(QVariant *data);
    virtual void apply_backend() override;

protected:
    QLabel *m_im_label{nullptr};
};

Port *create_port(
    const std::string &node_id, uint index,
    const std::string &name,
    Port::Type port_type,
    Port::DataType data_type,
    std::vector<std::string> items);