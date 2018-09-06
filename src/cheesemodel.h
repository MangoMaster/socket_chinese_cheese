#ifndef CHEESE_MODEL_H_
#define CHEESE_MODEL_H_

#include <QtCore/QObject>
#include <QtCore/QSet>
#include <cheese.h>

class CheeseModel : public QObject
{
  Q_OBJECT

public:
  CheeseModel(CheeseColor color, QObject *parent = nullptr);
  ~CheeseModel();

signals:
  void modelChanged(const std::array<std::array<Cheese *, 9>, 10> &changedCheese);
  void modelChanged(CheesePoint startCheesePoint, CheesePoint endCheesePoint);

public slots:
  void setNewModel();
  void receiveMousePress(CheesePoint cheesePoint);
  void receiveMousePress(); // clear chosen point

private:
  CheeseColor color;
  std::array<std::array<Cheese *, 9>, 10> cheeseTable;
  CheesePoint cheeseChosenPoint; // -1 -> not exist
  QSet<CheesePoint> cheeseNextPoint;

  void chooseCheese(const CheesePoint &cheesePoint);
  void clearChosenCheese();
  void chooseCheeseNextPoint(const CheesePoint &cheesePoint);
};

#endif // CHEESE_MODEL_H_