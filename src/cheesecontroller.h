#ifndef CHEESE_CONTROLLER_H_
#define CHEESE_CONTROLLER_H_

#include <QtCore/QObject>
#include "mainwindow.h"
#include "cheesescene.h"
#include "cheesemodel.h"

class CheeseController : public QObject
{
  Q_OBJECT

public:
  CheeseController(MainWindow *mainWindow, CheeseScene *scene, CheeseModel *model, QObject *parent = nullptr);

private:
  MainWindow *mainWindow;
  CheeseScene *scene;
  CheeseModel *model;

  void control();
};

#endif // CHEESE_CONTROLLER_H_