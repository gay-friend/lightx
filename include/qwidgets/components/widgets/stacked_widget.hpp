#pragma once
#include <QStackedWidget>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <vector>

/// @brief Stacked widget with fade in and fade out animation
class OpacityAniStackedWidget : public QStackedWidget
{
public:
    OpacityAniStackedWidget(QWidget *parent = nullptr) : QStackedWidget(parent)
    {
    }
    void addWidget(QWidget *w)
    {
        QStackedWidget::addWidget(w);

        auto effect = new QGraphicsOpacityEffect(this);
        effect->setOpacity(1);
        auto ani = new QPropertyAnimation(effect, "opacity", this);
        ani->setDuration(220);
        connect(ani, SIGNAL(ani->finished()), this, SLOT(m_on_ani_finished()));
        m_anis.push_back(ani);
        m_effects.push_back(effect);
        w->setGraphicsEffect(effect);
    }
    void setCurrentIndex(int index)
    {
        auto _index = currentIndex();
        if (index = _index)
        {
            return;
        }

        auto ani = m_anis[index];
        index > _index ? ani->setStartValue(0) : ani->setStartValue(1);
        index > _index ? ani->setEndValue(1) : ani->setEndValue(0);
        if (index > _index)
        {
            setCurrentIndex(index);
        }

        widget(_index)->show();
        m_next_index = index;
        ani->start();
    }
    void setCurrentWidget(QWidget *w)
    {
        setCurrentIndex(indexOf(w));
    }

protected:
    void m_on_ani_finished()
    {
        QStackedWidget::setCurrentIndex(m_next_index);
    }
    int m_next_index{0};
    std::vector<QGraphicsOpacityEffect *> m_effects;
    std::vector<QPropertyAnimation *> m_anis;
};

class PopUpAniInfo
{
public:
    PopUpAniInfo(QWidget *widget, int delta_x, int delta_y, QPropertyAnimation *ani)
        : widget(widget), delta_x(delta_x), delta_y(delta_y), ani(ani)
    {
    }
    QWidget *widget;
    int delta_x;
    int delta_y;
    QPropertyAnimation *ani;
};

class PopUpAniStackedWidget : public QStackedWidget
{
    Q_OBJECT
public:
    PopUpAniStackedWidget(QWidget *parent = nullptr) : QStackedWidget(parent)
    {
    }
    void addWidget(QWidget *w, int delta_x = 0, int delta_y = 76)
    {
        QStackedWidget::addWidget(w);
        m_ani_infos.push_back(PopUpAniInfo(w, delta_x, delta_y, new QPropertyAnimation(w, "pos")));
    }
    void setCurrentIndex(int index, int duration = 250, bool pop_out = false,
                         bool show_next_directly = true, QEasingCurve easing = QEasingCurve::OutQuad)
    {
        if (index < 0 || index >= count())
        {
            return;
        }
        if (index == currentIndex())
        {
            return;
        }

        if (m_ani != nullptr && m_ani->state() == QAbstractAnimation::Running)
        {
            m_ani->stop();
            m_on_ani_finished();
        }

        m_next_index = index;

        auto cur_info = m_ani_infos[currentIndex()];
        auto next_info = m_ani_infos[index];
        auto cur_widget = currentWidget();
        auto next_widget = next_info.widget;
        m_ani = pop_out ? cur_info.ani : next_info.ani;
        QPoint pos;
        if (pop_out)
        {
            pos = cur_widget->pos() + QPoint(cur_info.delta_x, cur_info.delta_y);
            m_set_animation(m_ani, cur_widget->pos(), pos, duration, easing);
            next_widget->setVisible(show_next_directly);
        }
        else
        {
            pos = next_widget->pos() + QPoint(next_info.delta_x, next_info.delta_y);
            m_set_animation(m_ani, pos, QPoint(next_widget->x(), 0), duration, easing);
            QStackedWidget::setCurrentIndex(index);
        }
        connect(m_ani, SIGNAL(m_ani->finished()), this, SLOT(m_on_ani_finished()));
        m_ani->start();
        emit ani_start();
    }
    void setCurrentWidget(QWidget *w, int duration = 250, bool pop_out = false,
                          bool show_next_directly = true, QEasingCurve easing = QEasingCurve::OutQuad)
    {
        setCurrentIndex(indexOf(w), duration, pop_out, show_next_directly, easing);
    }
signals:
    void ani_finished();
    void ani_start();

protected:
    void m_set_animation(QPropertyAnimation *ani, QPoint start, QPoint end, int duration, QEasingCurve easing = QEasingCurve::Linear)
    {
        ani->setEasingCurve(easing);
        ani->setStartValue(start);
        ani->setEndValue(end);
        ani->setDuration(duration);
    }
    void m_on_ani_finished()
    {
        m_ani->disconnect();
        QStackedWidget::setCurrentIndex(m_next_index);
        emit ani_finished();
    }
    std::vector<PopUpAniInfo> m_ani_infos{};
    int m_next_index{0};
    QPropertyAnimation *m_ani{nullptr};
};