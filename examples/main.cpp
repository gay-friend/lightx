#include "qwidgets/components/widgets/stacked_widget.hpp"
#include "qwidgets/components/widgets/label.hpp"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    auto w = new PopUpAniStackedWidget();
    auto label2 = widgets::StrongBodyLabel("StrongBodyLabel", w);
    w->show();
    return app.exec();
}