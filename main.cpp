#include "mainwindow.h"
#include "cheesescene.h"
#include "cheesemodel.h"
#include "cheesecontroller.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow w;
    CheeseScene s(CheeseColor::black);
    CheeseModel m(CheeseColor::black);
    CheeseController c(&w, &s, &m);
    w.show();

    return app.exec();
}