#ifndef CHEESE_MODEL_H_
#define CHEESE_MODEL_H_

#include <QtCore/QObject>
#include <QtCore/QSet>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QtMultimedia/QMediaPlayer>
#include "cheese.h"
#include "cheesetcpconnection.h"

class CheeseModel : public QObject
{
  Q_OBJECT

public:
  explicit CheeseModel(QObject *parent = nullptr);
  ~CheeseModel();

signals:
  void modelChanged(const std::array<std::array<Cheese *, 9>, 10> &changedCheese, CheeseColor myColor); // start model
  void modelChanged(CheesePoint startCheesePoint, CheesePoint endCheesePoint);                          // change model
  void colorChanged(CheeseColor nextStepColor);
  void gameEnded();
  void readySend(const std::array<std::array<Cheese *, 9>, 10> &changedCheese, CheeseColor currentColor);
  void readySend(CheesePoint startCheesePoint, CheesePoint endCheesePoint);
  void readySend(CheeseColor winColor);

public slots:
  void setNewModel();
  void setPiecesModel();
  void setJoinModel();
  void saveModel();
  void setLoseGame();
  void setEndGame();
  void receiveMousePress(CheesePoint cheesePoint);
  void receiveMousePress(); // clear chosen point
  void receiveRecv(std::array<std::array<Cheese *, 9>, 10> changedCheese, CheeseColor currentColor);
  void receiveRecv(CheesePoint startCheesePoint, CheesePoint endCheesePoint);
  void receiveRecv(CheeseColor winColor);

private:
  bool gaming = false; // 游戏中
  QString mode;        // 游戏模式
  CheeseColor myCheeseColor;
  CheeseColor currentStepColor;
  CheesePoint redShuaiPoint;
  CheesePoint blackShuaiPoint;
  std::array<std::array<Cheese *, 9>, 10> cheeseTable;
  CheesePoint cheeseChosenPoint; // -1 -> not exist
  QSet<CheesePoint> cheeseNextPoint;

  void chooseCheese(const CheesePoint &cheesePoint);
  void clearChosenCheese();
  void chooseCheeseNextPoint(const CheesePoint &cheesePoint);
  void goCheese(CheesePoint startCheesePoint, CheesePoint endCheesePoint);
  void checkJiangJun();
  void enterNextStep();
  void endGame(CheeseColor winColor);
  bool checkGoable(const CheesePoint &startCheesePoint, const CheesePoint &endCheesePoint);

  static const CheesePoint MA_POINT_DELTA[8];
  static const CheesePoint MA_BIE_POINT_DELTA[8];
  static const CheesePoint XIANG_POINT_DELTA[4];
  static const CheesePoint XIANG_BIE_POINT_DELTA[4];
  static const CheesePoint SHI_POINT_DELTA[4];
  static const CheesePoint SHUAI_POINT_DELTA[4];
  static const CheesePoint BING_RED_POINT_DELTA[3];
  static const CheesePoint BING_BLACK_POINT_DELTA[3];

  CheeseTcpConnection *connection = nullptr;
  void connectToConnection();

  QMediaPlayer *jiangJunPlayer;
};

#endif // CHEESE_MODEL_H_