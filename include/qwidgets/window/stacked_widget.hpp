#pragma once
#include <QHBoxLayout>
#include <QAbstractScrollArea>
#include <QScrollBar>
#include "qwidgets/components/widgets/stacked_widget.hpp"

class StackedWidget : public QFrame
{
public:
    StackedWidget(QWidget *parent) : QFrame(parent)
    {
        h_layout = new QHBoxLayout(this);
        view = new PopUpAniStackedWidget(this);

        h_layout->setContentsMargins(0, 0, 0, 0);
        h_layout->addWidget(view);

        connect(view, SIGNAL(view->currentChanged()), this, SLOT(index_changed()));
        setAttribute(Qt::WidgetAttribute::WA_StyledBackground);
    }
    void addWidget(QWidget *w)
    {
        view->addWidget(w);
    }

    QWidget *widget(int index)
    {
        return view->widget(index);
    }

    void setCurrentWidget(QWidget *w, bool pop_out)
    {
        if (typeid(QAbstractScrollArea *) == typeid(w))
        {
            auto _w = dynamic_cast<QAbstractScrollArea *>(w);
            _w->verticalScrollBar()->setValue(0);
        }

        if (pop_out)
        {
            view->setCurrentWidget(w, 300);
        }
        else
        {
            view->setCurrentWidget(w, 300, true, false, QEasingCurve::InQuad);
        }
    }
    void setCurrentIndex(int index, bool pop_out)
    {
        setCurrentWidget(widget(index), pop_out);
    }
    int currentIndex()
    {
        return view->currentIndex();
    }
    int indexOf(QWidget *w)
    {
        return view->indexOf(w);
    }
    int count()
    {
        return view->count();
    }

    QHBoxLayout *h_layout;
    PopUpAniStackedWidget *view;
signals:
    void index_changed(int);
};