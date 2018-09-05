#ifndef CHEESE_SCENE_H_
#define CHEESE_SCENE_H_

#include <QtWidgets/QGraphicsScene>

class CheeseScene : public QGraphicsScene
{
  public:
    CheeseScene(QObject *parent = nullptr);
};

#endif // CHEESE_SCENE_H_