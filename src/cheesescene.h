#ifndef CHEESE_SCENE_H_
#define CHEESE_SCENE_H_

#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsPixmapItem>
#include <QtCore/QSet>
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
  CheeseScene(QObject *parent = nullptr);
  ~CheeseScene();

signals:
  void mousePressed(); // invalid mouse press
  void mousePressed(CheesePoint cheesePoint);

public slots:
  void receiveModel(const std::array<std::array<Cheese *, 9>, 10> &changedCheese, CheeseColor color); // start model
  void receiveModel(CheesePoint startCheesePoint, CheesePoint endCheesePoint);                        // change model
  void receiveNextPoint(QSet<CheesePoint> nextPoint);

protected:
  void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
  CheeseColor myCheeseColor;
  std::array<std::array<CheesePixmap *, 9>, 10> cheesePixmapTable;
  std::array<std::array<QGraphicsRectItem *, 9>, 10> rectTable;
  QSet<CheesePoint> cheeseNextPoint;
};

#endif // CHEESE_SCENE_H_