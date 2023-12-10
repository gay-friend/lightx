#include "widget/mainwidow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    (new MainWindow())->show();
    return app.exec();
}