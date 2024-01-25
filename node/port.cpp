#include "node/port.h"

Port::Port(const std::string &node_id, uint index,
           const std::string &name, Type type, QColor color)
    : node_id(node_id), index(index), name(name), type(type), color(color), QGraphicsObject(nullptr)
{
    m_pen_default = QPen(color);
    m_pen_default.setWidthF(1.5);
    m_brush_default = QBrush(color);
    m_font = QFont("Consolas", m_font_size);
    m_label_size = QFontMetrics(m_font).horizontalAdvance(QString::fromStdString(name));
    port_width = icon_size + m_label_size;
    m_data = new QVariant();

    this->uuid = generate_uuid();
    QObject::connect(this, &Port::on_value_change, this, &Port::apply_ui);
}
void Port::loads(json config)
{
    this->name = config["name"];
    this->type = config["type"];
    this->uuid = config["uuid"];
    this->data_type = config["data_type"];
}

json Port::dumps()
{
    json config;
    config["index"] = this->index;
    config["name"] = this->name;
    config["type"] = this->type;
    config["data_type"] = this->data_type;
    config["uuid"] = this->uuid;
    return config;
}

QPointF Port::get_port_pos() const
{
    auto pos = scenePos();
    if (this->data_type == Port::Output)
    {
        return QPointF(pos.x() + m_label_size + 0.5 * icon_size, pos.y() + 0.5 * icon_size);
    }
    else
    {
        return QPointF(pos.x() + 0.25 * icon_size, pos.y() + 0.5 * icon_size);
    }
}
QRectF Port::boundingRect() const
{
    return QRectF(0, 0, port_width, icon_size);
}

void Port::disconnect()
{
    is_connected = false;
    if (type != Output && m_parent != nullptr)
    {
        QObject::disconnect(m_parent, &Port::on_value_change, this, &Port::on_value_change);
        m_parent = nullptr;
    }
}

void Port::connect(Port *port)
{
    if (port == this)
    {
        return;
    }
    is_connected = true;
    port->is_connected = true;

    type != Output ? set_parent(port) : port->set_parent(this);
}
bool Port::readonly()
{
    return m_parent != nullptr;
}

bool Port::is_pair()
{
    return this->type == Port::Output && m_parent != nullptr;
}

void Port::set_parent(Port *port)
{
    QObject::connect(port, &Port::on_value_change, this, &Port::on_value_change);
    m_parent = port;
}
void Port::add_child(Port *port)
{
    if (port != nullptr)
    {
        this->m_childs.push_back(port);
    }
}
void Port::remove_child(Port *port)
{
    auto it = std::find(this->m_childs.begin(), this->m_childs.end(), port);
    if (it != this->m_childs.end())
    {
        m_childs.erase(it);
    }
}

void Port::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // icon o-> 来表示
    if (is_connected)
    {
        painter->setPen(Qt::NoPen);
        painter->setBrush(m_brush_default);
    }
    else
    {
        painter->setPen(m_pen_default);
        painter->setBrush(Qt::NoBrush);
    }

    auto size = 0.25 * icon_size;

    QPolygonF poly;
    if (Port::Output == this->data_type)
    {
        painter->drawEllipse(
            QPointF(m_label_size + 0.5 * icon_size, 0.5 * icon_size),
            0.25 * icon_size,
            0.25 * icon_size);
        poly.append(QPointF(m_label_size + 0.85 * icon_size, 0.35 * icon_size));
        poly.append(QPointF(m_label_size + 1 * icon_size, 0.5 * icon_size));
        poly.append(QPointF(m_label_size + 0.85 * icon_size, 0.65 * icon_size));
    }
    else
    {
        painter->drawEllipse(QPointF(size, 2 * size), size, size);
        poly.append(QPointF(0.6 * icon_size, 0.35 * icon_size));
        poly.append(QPointF(0.75 * icon_size, 0.5 * icon_size));
        poly.append(QPointF(0.6 * icon_size, 0.65 * icon_size));
    }
    painter->setBrush(m_brush_default);
    painter->setPen(Qt::NoPen);
    painter->drawPolygon(poly);

    // port label
    painter->setPen(m_pen_default);
    painter->setFont(m_font);
    if (Port::Output == this->data_type)
    {
        painter->drawText(
            QRectF(0, 0, m_label_size, icon_size),
            Qt::AlignmentFlag::AlignRight | Qt::AlignmentFlag::AlignVCenter,
            QString::fromStdString(name));
    }
    else
    {
        painter->drawText(
            QRectF(icon_size, 0, m_label_size, icon_size),
            Qt::AlignmentFlag::AlignLeft | Qt::AlignmentFlag::AlignVCenter,
            QString::fromStdString(name));
    }
}

// ================================StringPort====================================
StringPort::StringPort(const std::string &node_id, uint index,
                       const std::string &name, Type type, QColor color)
    : Port(node_id, index, name, type, color)
{
    this->data_type = Port::String;
    if (this->is_pair())
    {
        return;
    }
    this->setting_layout = new QHBoxLayout();
    this->m_edit = new QLineEdit();
    auto label = new QLabel(QString::fromStdString(this->name));
    this->m_edit->setReadOnly(this->readonly());

    this->setting_layout->addWidget(label);
    this->setting_layout->addWidget(this->m_edit);
}
void StringPort::loads(json config)
{
    this->set_value<std::string>(config["value"]);
}
json StringPort::dumps()
{
    auto config = Port::dumps();
    config["value"] = this->get_value<std::string>();
    return config;
}

void StringPort::apply_ui(QVariant *data)
{
    if (data == nullptr || this->m_edit == nullptr)
    {
        return;
    }
    this->m_edit->setText(data->toString());
}
void StringPort::apply_backend()
{
    if (this->m_edit == nullptr)
    {
        return;
    }
    this->set_value(this->m_edit->text().toStdString());
}

// ================================FilePort====================================
FilePort::FilePort(const std::string &node_id, uint index,
                   const std::string &name, Type type, QColor color)
    : StringPort(node_id, index, name, type, color)
{
    this->data_type = Port::File;
}

// ================================IntPort====================================
IntPort::IntPort(const std::string &node_id, uint index,
                 const std::string &name, Type type, QColor color)
    : StringPort(node_id, index, name, type, color)
{
    this->data_type = Port::Int;
    if (this->is_pair())
    {
        return;
    }
    this->m_edit->setValidator(new QIntValidator());
}
void IntPort::loads(json config)
{
    this->set_value<int>(config["value"]);
}
void IntPort::apply_backend()
{
    if (this->m_edit == nullptr)
    {
        return;
    }
    this->set_value(this->m_edit->text().toInt());
}

json IntPort::dumps()
{
    auto config = Port::dumps();
    config["value"] = this->get_value<int>();
    return config;
}
// ================================FloatPort====================================
FloatPort::FloatPort(const std::string &node_id, uint index,
                     const std::string &name, Type type, QColor color)
    : StringPort(node_id, index, name, type, color)
{
    this->data_type = Port::Float;
    if (this->is_pair())
    {
        return;
    }
    this->m_edit->setValidator(new QDoubleValidator());
}
void FloatPort::loads(json config)
{
    this->data_type = Port::Float;
    this->set_value<float>(config["value"]);
}
void FloatPort::apply_backend()
{
    if (this->m_edit == nullptr)
    {
        return;
    }
    this->set_value(this->m_edit->text().toFloat());
}

json FloatPort::dumps()
{
    auto config = Port::dumps();
    config["value"] = this->get_value<float>();
    return config;
}

// ================================BoolPort====================================
BoolPort::BoolPort(const std::string &node_id, uint index,
                   const std::string &name, Type type, QColor color)
    : Port(node_id, index, name, type, color)
{
    this->data_type = Port::Bool;
    if (this->is_pair())
    {
        return;
    }
    this->setting_layout = new QHBoxLayout();
    this->m_check = new QCheckBox();
    auto label = new QLabel(QString::fromStdString(this->name));
    this->m_check->setCheckable(!this->readonly());

    this->setting_layout->addWidget(label);
    this->setting_layout->addWidget(this->m_check);
}
void BoolPort::loads(json config)
{
    this->set_value<std::string>(config["value"]);
}
json BoolPort::dumps()
{
    auto config = Port::dumps();
    config["value"] = this->get_value<bool>();
    return config;
}

void BoolPort::apply_ui(QVariant *data)
{
    if (data == nullptr || this->m_check == nullptr)
    {
        return;
    }
    this->m_check->setChecked(data->value<bool>());
}
void BoolPort::apply_backend()
{
    if (this->is_pair())
    {
        return;
    }
    this->set_value(this->m_check->isChecked());
}

// ================================EnumPort====================================
EnumPort::EnumPort(const std::string &node_id,
                   uint index, const std::string &name,
                   Type type,
                   std::vector<std::string> items,
                   QColor color)
    : Port(node_id, index, name, type, color), m_combo_items(items)
{
    this->data_type = Port::Enum;
    if (this->is_pair())
    {
        return;
    }
    this->setting_layout = new QHBoxLayout();
    this->m_combo = new QComboBox();
    auto label = new QLabel(QString::fromStdString(this->name));
    this->m_combo->setEnabled(!this->readonly());
    for (auto item : this->m_combo_items)
    {
        this->m_combo->addItem(QString::fromStdString(item));
    }
    if (this->m_combo_items.size() > 0)
    {
        this->m_combo->setCurrentIndex(0);
        this->set_value(this->m_combo_items[0]);
    }

    this->setting_layout->addWidget(label);
    this->setting_layout->addWidget(this->m_combo);
}

void EnumPort::loads(json config)
{
    std::string value = config["value"];
    auto it = std::find(this->m_combo_items.begin(), this->m_combo_items.end(), value);
    if (it != this->m_combo_items.end())
    {
        this->set_value(value);
    }
}
json EnumPort::dumps()
{
    auto config = Port::dumps();
    config["value"] = this->get_value<std::string>();
    return config;
}

void EnumPort::apply_ui(QVariant *data)
{
    std::cout << "apply EnumPort backend: " << this->m_combo->currentText().toStdString() << std::endl;
    if (data == nullptr || this->m_combo == nullptr)
    {
        return;
    }
    auto value = data->value<std::string>();
    auto it = std::find(this->m_combo_items.begin(), this->m_combo_items.end(), value);
    if (it != this->m_combo_items.end())
    {
        this->m_combo->setCurrentText(QString::fromStdString(value));
    }
}
void EnumPort::apply_backend()
{
    if (this->m_combo == nullptr)
    {
        return;
    }
    std::cout << this->m_combo->currentText().toStdString() << std::endl;
    this->set_value(this->m_combo->currentText().toStdString());
}

// ================================ImagePort====================================
ImagePort::ImagePort(const std::string &node_id, uint index,
                     const std::string &name, Type type, QColor color)
    : Port(node_id, index, name, type, color)
{
    this->data_type = Port::Image;
    if (this->is_pair())
    {
        return;
    }
    this->setting_layout = new QHBoxLayout();
    this->m_im_label = new QLabel();
    auto label = new QLabel(QString::fromStdString(this->name));

    this->setting_layout->addWidget(label);
    this->setting_layout->addWidget(this->m_im_label);
}

void ImagePort::apply_ui(QVariant *data)
{
    if (data == nullptr || this->m_im_label == nullptr)
    {
        return;
    }
    auto mat = data->value<cv::Mat>();
    auto qim = mat_to_qimage(mat);
    this->m_im_label->setPixmap(QPixmap::fromImage(qim));
}
void ImagePort::apply_backend()
{
}

// =================================Create Port================================
Port *create_port(
    const std::string &node_id, uint index,
    const std::string &name,
    Port::Type port_type,
    Port::DataType data_type,
    std::vector<std::string> items)
{
    switch (data_type)
    {
    case Port::String:
        return new StringPort(node_id, index, name, port_type);
    case Port::Int:
        return new IntPort(node_id, index, name, port_type);
    case Port::Float:
        return new FloatPort(node_id, index, name, port_type);
    case Port::File:
        return new FilePort(node_id, index, name, port_type);
    case Port::Enum:
        return new EnumPort(node_id, index, name, port_type, items);
    case Port::Image:
        return new ImagePort(node_id, index, name, port_type);
    case Port::Bool:
        return new BoolPort(node_id, index, name, port_type);

    default:
        return nullptr;
    }
}