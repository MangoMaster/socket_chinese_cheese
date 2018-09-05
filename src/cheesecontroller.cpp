#include "cheesecontroller.h"
#include "ui_mainwindow.h"

CheeseController::CheeseController(MainWindow *mainWindow, CheeseScene *scene, CheeseModel *model, QObject *parent)
    : QObject(parent), mainWindow(mainWindow), scene(scene), model(model)
{
    this->control();
}

void CheeseController::control()
{
    // view & scene
    this->mainWindow->ui->graphicsView->setScene(this->scene);

    // mainWindow & model
    QObject::connect(this->mainWindow->ui->action_New, SIGNAL(triggered(bool)), this->model, SLOT(setNewModel()));

    // scene & model
    QObject::connect(this->model, SIGNAL(modelChanged(const std::array<std::array<Cheese *, 9>, 10> &)), this->scene, SLOT(receiveModel(const std::array<std::array<Cheese *, 9>, 10> &)));
}
