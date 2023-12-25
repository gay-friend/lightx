#pragma once
#include <QWidget>

namespace common
{
    QFont getFont(int fontSize = 14, QFont::Weight weight = QFont::Weight::Normal)
    {

        QFont font;
        font.setFamilies({"Segoe UI", "Microsoft YaHei", "PingFang SC"});
        font.setPixelSize(fontSize);
        font.setWeight(weight);
        return font;
    }

    void setFont(QWidget *widget, int fontSize = 14, QFont::Weight weight = QFont::Weight::Normal)
    {
        widget->setFont(getFont(fontSize, weight));
    }

}
