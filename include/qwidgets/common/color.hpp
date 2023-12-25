#pragma once
#include <QColor>
namespace common
{
    QColor mixColor(QColor c1, QColor c2, float weight)
    {
        int r = c1.red() * weight + c2.red() * (1 - weight);
        int g = c1.green() * weight + c2.green() * (1 - weight);
        int b = c1.blue() * weight + c2.blue() * (1 - weight);
        return QColor(r, g, b);
    }

    QColor mixLight(QColor color, float weight)
    {
        return mixColor(color, QColor(255, 255, 255), weight);
    }
    QColor mixDark(QColor color, float weight)
    {
        return mixColor(color, QColor(0, 0, 0), weight);
    }
    QColor translucent(QColor color, int alpha)
    {
        return QColor(color.red(), color.green(), color.blue(), alpha);
    }

} // namespace common
