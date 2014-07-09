#include <QMainWindow>
#include <cvWindow.h>
#include "mainwindow.h"
#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    //MainWindow w;
    cvWindow win;
    win.show();

    QObject::connect(&win, SIGNAL(closed()),
                     &app, SLOT(quit()) );

    return app.exec();
}
