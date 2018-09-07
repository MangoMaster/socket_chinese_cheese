#include "cheeseview.h"

CheeseView::CheeseView(QWidget *parent)
    : QGraphicsView(parent)
{
}

void CheeseView::setScalePercent(int percent)
{
    this->resetTransform();
    this->scale(percent * 0.01, percent * 0.01);
}
