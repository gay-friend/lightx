#include "widgets/editor.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    (new EditorWindow())->show();
    return app.exec();
}