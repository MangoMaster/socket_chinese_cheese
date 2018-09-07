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
    QObject::connect(this->mainWindow->ui->action_Join, SIGNAL(triggered(bool)), this->model, SLOT(setJoinModel()));
    QObject::connect(this->mainWindow->ui->action_Pieces, SIGNAL(triggered(bool)), this->model, SLOT(setPiecesModel()));
    QObject::connect(this->mainWindow->ui->action_Save, SIGNAL(triggered(bool)), this->model, SLOT(saveModel()));
    QObject::connect(this->mainWindow->ui->action_Exit, SIGNAL(triggered(bool)), qApp, SLOT(quit()));
    QObject::connect(this->mainWindow->ui->pushButton, SIGNAL(clicked()), this->model, SLOT(setLoseGame()));
    QObject::connect(this->mainWindow, SIGNAL(timerTimeout()), this->model, SLOT(setEndGame()));
    QObject::connect(this->model, SIGNAL(colorChanged(CheeseColor)), this->mainWindow, SLOT(timerZero()));
    QObject::connect(this->model, SIGNAL(gameEnded()), this->mainWindow, SLOT(timerStop()));

    // scene & model
    QObject::connect(this->model, SIGNAL(modelChanged(const std::array<std::array<Cheese *, 9>, 10> &, CheeseColor)),
                     this->scene, SLOT(receiveModel(const std::array<std::array<Cheese *, 9>, 10> &, CheeseColor)));
    QObject::connect(this->model, SIGNAL(modelChanged(CheesePoint, CheesePoint)), this->scene, SLOT(receiveModel(CheesePoint, CheesePoint)));
    QObject::connect(this->scene, SIGNAL(mousePressed(CheesePoint)), this->model, SLOT(receiveMousePress(CheesePoint)));
    QObject::connect(this->scene, SIGNAL(mousePressed()), this->model, SLOT(receiveMousePress()));
}
