#include "widgets/drag_label.h"

DragLabel::DragLabel(const std::string &text, QWidget *parent)
    : QLabel(QString::fromStdString(text), parent)
{
}

void DragLabel::mouseMoveEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        return;
    }

    auto mime_data = new QMimeData();
    mime_data->setParent(this);
    auto drag = QDrag(this);
    drag.setMimeData(mime_data);
    drag.exec(Qt::MoveAction);
}
