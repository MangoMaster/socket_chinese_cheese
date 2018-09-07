#ifndef CHEESE_VIEW_H_
#define CHEESE_VIEW_H_

#include <QtWidgets/QGraphicsView>

class CheeseView : public QGraphicsView
{
  Q_OBJECT

public:
  CheeseView(QWidget *parent = nullptr);

public slots:
  void setScalePercent(int percent);
};

#endif // CHEESE_VIEW_H_