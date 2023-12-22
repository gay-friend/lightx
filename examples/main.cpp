#include "qwidgets/components/widgets/stacked_widget.hpp"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    (new PopUpAniStackedWidget())->show();
    return app.exec();
}