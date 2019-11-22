#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    RustIDE::MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
