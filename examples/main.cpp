#include "qwidgets/components/widgets/stacked_widget.hpp"
#include "qwidgets/components/widgets/label.hpp"

#include <QApplication>
#include <opencv2/opencv.hpp>

int main(int argc, char *argv[])
{
    auto mat = cv::imread("/home/lightx/lightx/assets/images/0b989efe6459e72254b78371ced8bd13.png");
    auto var = new QVariant();
    var->setValue(QVariant::fromValue(mat));
    cv::imwrite("mat.png", mat);

    QApplication app(argc, argv);
    auto w = new PopUpAniStackedWidget();
    widgets::StrongBodyLabel("StrongBodyLabel", w);
    w->show();
    return app.exec();
}