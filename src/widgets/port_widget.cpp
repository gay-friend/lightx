#include "widgets/port_widget.h"

PortWidget::PortWidget(Port *port, QWidget *parent) : QWidget(parent), m_port(port)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedWidth(60);
}

InputWidget::InputWidget(Port *port, QWidget *parent) : PortWidget(port, parent)
{
    m_vbox_layout = new QVBoxLayout(this);
    m_vbox_layout->setContentsMargins(0, 0, 0, 0);
    m_text_edit = new QLineEdit(this);
    switch (port->data_type)
    {
    case Port::Int:
        m_text_edit->setValidator(new QDoubleValidator());
        break;
    case Port::Float:
        m_text_edit->setValidator(new QIntValidator());
        break;

    default:
        break;
    }
    m_vbox_layout->addWidget(m_text_edit);
}

IntInput::IntInput(Port *port, QWidget *parent) : InputWidget(port, parent)
{
}
FloatInput::FloatInput(Port *port, QWidget *parent) : InputWidget(port, parent)
{
}
StringInput::StringInput(Port *port, QWidget *parent) : InputWidget(port, parent)
{
}

CheckboxInput::CheckboxInput(Port *port, QWidget *parent) : PortWidget(port, parent)
{
    m_vbox_layout = new QVBoxLayout(this);
    m_vbox_layout->setContentsMargins(0, 0, 0, 0);
    m_checkbox = new QCheckBox(this);
    setFixedWidth(30);
    setStyleSheet("height:25px;width:25px;");
    m_vbox_layout->addWidget(m_checkbox);
}

FileInput::FileInput(Port *port, QWidget *parent) : PortWidget(port, parent)
{
    setFixedWidth(40);
    m_vbox_layout = new QVBoxLayout(this);
    m_vbox_layout->setContentsMargins(0, 0, 0, 0);
    m_select_btn = new QPushButton("+", this);
    m_vbox_layout->addWidget(m_select_btn);

    // connect solt
    m_select_btn->setStyleSheet("font-weight:bold;font-size:18px;");
}