#pragma once

#include <QIconEngine>
#include "qwidgets/common/config.hpp"

namespace common
{
    class SvgIconEngine : public QIconEngine
    {
    public:
        SvgIconEngine(std::string svg) : QIconEngine(), m_svg(svg)
        {
        }
        virtual void paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state)
        {
            QSvgRenderer(m_svg.c_str());
        }

    protected:
        std::string m_svg{""};
    };

    class MaterialIconBase
    {
    public:
        virtual std::string path(common::QConfig::Theme theme) = 0;
        QIcon icon(common::QConfig::Theme theme, QColor color)
        {
            auto color_str = color.name();
            return QIcon();
        }
    };

    class Icon : public QIcon
    {
    public:
    };

    class MaterialIconEngine : public QIconEngine
    {
    public:
        MaterialIconEngine() : QIconEngine()
        {
        }
    };

} // namespace common
