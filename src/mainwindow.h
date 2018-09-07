#ifndef MAIN_WINDOW_H_
#define MAIN_WINDOW_H_

#include <QtWidgets/QMainWindow>
#include <QtCore/QTimer>
#include "cheese.h"

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();
  friend class CheeseController;

signals:
  void timerTimeout();

public slots:
  void timerStart();
  void timerStop();
  void timerZero();
  void timerSingalTimeOut();
  void setCurrentColor(CheeseColor currentColor);

protected:
  void closeEvent(QCloseEvent *event) override;

private:
  Ui::MainWindow *ui;
  bool end = false;
  QTimer t;
  int time = 60;
};

#endif // MAIN_WINDOW_H_