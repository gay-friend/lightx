#include "qwidgets/components/widgets/stacked_widget.hpp"
#include "qwidgets/components/widgets/label.hpp"

#include <iostream>
#include <QApplication>
#include <opencv2/opencv.hpp>
#include <nlohmann/json.hpp>
#include <QLibrary>
#include <QDebug>
using json = nlohmann::json;

int main(int argc, char *argv[])
{
    // auto mat = cv::imread("/home/lightx/lightx/assets/images/0b989efe6459e72254b78371ced8bd13.png");
    // auto var = new QVariant();
    // var->setValue(QVariant::fromValue(mat));
    // cv::imwrite("mat.png", mat);

    // QApplication app(argc, argv);
    // auto w = new PopUpAniStackedWidget();
    // widgets::StrongBodyLabel("StrongBodyLabel", w);
    // w->show();
    // return app.exec();
    QLibrary lib("test.dll");
    typedef void (*hello_func)();
    typedef int(my_val);
    lib.load();
    qDebug() << lib.errorString();

    auto h = (hello_func)lib.resolve("hello");
    h();
    // std::cout << lib.errorString().toStdString() << std::endl;

    json obj;
    obj["id"] = "1234";
    obj["a"] = 1;
    json obj1;
    obj1["arr"] = {obj, obj};
    std::cout << obj1 << std::endl;
    return 0;
}