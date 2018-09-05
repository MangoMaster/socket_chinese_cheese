#ifndef CHEESE_SCENE_H_
#define CHEESE_SCENE_H_

#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsPixmapItem>
#include <cheese.h>

class CheesePixmap : public QGraphicsPixmapItem
{
public:
  explicit CheesePixmap(QGraphicsItem *parent = nullptr)
      : QGraphicsPixmapItem(parent)
  {
    this->setFlag(QGraphicsItem::ItemIsSelectable);
  }
};

class CheeseScene : public QGraphicsScene
{
  Q_OBJECT

public:
  CheeseScene(CheeseColor color, QObject *parent = nullptr);
  ~CheeseScene();

public slots:
  void receiveModel(const std::array<std::array<Cheese *, 9>, 10> &changedCheese);

private:
  CheeseColor color;
  std::array<std::array<CheesePixmap *, 9>, 10> cheesePixmapTable;
};

#endif // CHEESE_SCENE_H_