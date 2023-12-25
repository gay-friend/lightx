#pragma once

#include <QVBoxLayout>
#include <QWidget>
#include <QLineEdit>
#include <QCheckBox>
#include <QValidator>
#include <QPushButton>

#include "nodes/port.h"

class PortWidget : public QWidget
{
public:
    PortWidget(Port *port, QWidget *parent = nullptr);

protected:
    Port *m_port;
    uint m_icon_size{25};
};

class InputWidget : public PortWidget
{
public:
    InputWidget(Port *port, QWidget *parent = nullptr);

protected:
    QVBoxLayout *m_vbox_layout;
    QLineEdit *m_text_edit;
};

class IntInput : public InputWidget
{
public:
    IntInput(Port *port, QWidget *parent = nullptr);
};

class FloatInput : public InputWidget
{
public:
    FloatInput(Port *port, QWidget *parent = nullptr);
};

class StringInput : public InputWidget
{
public:
    StringInput(Port *port, QWidget *parent = nullptr);
};

class CheckboxInput : public PortWidget
{
public:
    CheckboxInput(Port *port, QWidget *parent = nullptr);

protected:
    QVBoxLayout *m_vbox_layout;
    QCheckBox *m_checkbox;
};

class FileInput : public PortWidget
{
public:
    FileInput(Port *port, QWidget *parent = nullptr);

protected:
    QVBoxLayout *m_vbox_layout;
    QPushButton *m_select_btn;
    std::string m_path{""};
};