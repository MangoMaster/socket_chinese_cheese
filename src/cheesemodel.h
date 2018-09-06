#ifndef CHEESE_MODEL_H_
#define CHEESE_MODEL_H_

#include <QtCore/QObject>
#include <QtCore/QSet>
#include <cheese.h>

class CheeseModel : public QObject
{
  Q_OBJECT

public:
  CheeseModel(QObject *parent = nullptr);
  ~CheeseModel();

signals:
  void modelChanged(const std::array<std::array<Cheese *, 9>, 10> &changedCheese, CheeseColor color); // start model
  void modelChanged(CheesePoint startCheesePoint, CheesePoint endCheesePoint);                        // change model
  void nextStep(CheeseColor nextStepColor);

public slots:
  void setNewModel();
  void receiveMousePress(CheesePoint cheesePoint);
  void receiveMousePress(); // clear chosen point

private:
  CheeseColor myCheeseColor;
  CheeseColor currentStepColor;
  std::array<std::array<Cheese *, 9>, 10> cheeseTable;
  CheesePoint cheeseChosenPoint; // -1 -> not exist
  QSet<CheesePoint> cheeseNextPoint;

  void chooseCheese(const CheesePoint &cheesePoint);
  void clearChosenCheese();
  void chooseCheeseNextPoint(const CheesePoint &cheesePoint);

  static const CheesePoint MA_POINT_DELTA[8];
  static const CheesePoint MA_BIE_POINT_DELTA[8];
  static const CheesePoint XIANG_POINT_DELTA[4];
  static const CheesePoint XIANG_BIE_POINT_DELTA[4];
  static const CheesePoint SHI_POINT_DELTA[4];
  static const CheesePoint SHUAI_POINT_DELTA[4];
  static const CheesePoint BING_RED_POINT_DELTA[3];
  static const CheesePoint BING_BLACK_POINT_DELTA[3];
};

#endif // CHEESE_MODEL_H_