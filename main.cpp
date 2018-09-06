#include "mainwindow.h"
#include "cheesescene.h"
#include "cheesemodel.h"
#include "cheesecontroller.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow w;
    CheeseScene s;
    CheeseModel m;
    CheeseController c(&w, &s, &m);
    w.show();

    return app.exec();
}