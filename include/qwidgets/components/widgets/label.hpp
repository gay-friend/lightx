#pragma once

#include <QLabel>
#include "qwidgets/common/config.hpp"
#include "qwidgets/common/font.hpp"
namespace widgets
{

    class MaterialLabelBase : public QLabel
    {
    public:
        MaterialLabelBase(QString text = "", QWidget *parent = nullptr) : QLabel(parent)
        {
            setText(text);
            setTextColor();
        }
        void setTextColor(QColor light = QColor(0, 0, 0), QColor dark = QColor(255, 255, 255))
        {
            lightColor = light;
            darkColor = dark;

            auto p = palette();
            auto color = common::isDarkTheme() ? darkColor : lightColor;
            p.setColor(QPalette::ColorRole::WindowText, color);
            setPalette(p);
        }

        virtual QFont getFont() = 0;
        QColor lightColor;
        QColor darkColor;
    };

    class CaptionLabel : public MaterialLabelBase
    {
    public:
        CaptionLabel(QString text = "", QWidget *parent = nullptr) : MaterialLabelBase(text, parent)
        {
            setFont(getFont());
        }
        virtual QFont getFont() override
        {
            return common::getFont(14);
        }
    };

    class BodyLabel : public MaterialLabelBase
    {
    public:
        BodyLabel(QString text = "", QWidget *parent = nullptr) : MaterialLabelBase(text, parent)
        {
            setFont(getFont());
        }
        virtual QFont getFont() override
        {
            return common::getFont(14);
        }
    };

    class StrongBodyLabel : public MaterialLabelBase
    {
    public:
        StrongBodyLabel(QString text = "", QWidget *parent = nullptr) : MaterialLabelBase(text, parent)
        {
            setFont(getFont());
        }
        virtual QFont getFont() override
        {
            return common::getFont(14, QFont::Weight::DemiBold);
        }
    };

    class SubtitleLabel : public MaterialLabelBase
    {
    public:
        SubtitleLabel(QString text = "", QWidget *parent = nullptr) : MaterialLabelBase(text, parent)
        {
            setFont(getFont());
        }
        virtual QFont getFont() override
        {
            return common::getFont(20, QFont::Weight::DemiBold);
        }
    };

    class TitleLabel : public MaterialLabelBase
    {
    public:
        TitleLabel(QString text = "", QWidget *parent = nullptr) : MaterialLabelBase(text, parent)
        {
            setFont(getFont());
        }
        virtual QFont getFont() override
        {
            return common::getFont(28, QFont::Weight::DemiBold);
        }
    };

    class LargeTitleLabel : public MaterialLabelBase
    {
    public:
        LargeTitleLabel(QString text = "", QWidget *parent = nullptr) : MaterialLabelBase(text, parent)
        {
            setFont(getFont());
        }
        virtual QFont getFont() override
        {
            return common::getFont(40, QFont::Weight::DemiBold);
        }
    };

    class DisplayLabel : public MaterialLabelBase
    {
    public:
        DisplayLabel(QString text = "", QWidget *parent = nullptr) : MaterialLabelBase(text, parent)
        {
            setFont(getFont());
        }
        virtual QFont getFont() override
        {
            return common::getFont(68, QFont::Weight::DemiBold);
        }
    };
} // namespace widgets
