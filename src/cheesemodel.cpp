#include "cheesemodel.h"
#include <cassert>

CheeseModel::CheeseModel(CheeseColor color, QObject *parent)
    : QObject(parent), color(color), cheeseChosenPoint(-1, -1)
{
    for (auto &cheeseArray : this->cheeseTable)
        for (auto &cheese : cheeseArray)
            cheese = nullptr;
}

CheeseModel::~CheeseModel()
{
    for (auto &cheeseArray : this->cheeseTable)
        for (auto &cheese : cheeseArray)
            delete cheese;
}

void CheeseModel::setNewModel()
{
    // black
    cheeseTable[0][0] = new Cheese(CheeseColor::black, CheeseKind::che, 0, 0);
    cheeseTable[0][1] = new Cheese(CheeseColor::black, CheeseKind::ma, 0, 1);
    cheeseTable[0][2] = new Cheese(CheeseColor::black, CheeseKind::xiang, 0, 2);
    cheeseTable[0][3] = new Cheese(CheeseColor::black, CheeseKind::shi, 0, 3);
    cheeseTable[0][4] = new Cheese(CheeseColor::black, CheeseKind::shuai, 0, 4);
    cheeseTable[0][5] = new Cheese(CheeseColor::black, CheeseKind::shi, 0, 5);
    cheeseTable[0][6] = new Cheese(CheeseColor::black, CheeseKind::xiang, 0, 6);
    cheeseTable[0][7] = new Cheese(CheeseColor::black, CheeseKind::ma, 0, 7);
    cheeseTable[0][8] = new Cheese(CheeseColor::black, CheeseKind::che, 0, 8);
    cheeseTable[2][1] = new Cheese(CheeseColor::black, CheeseKind::pao, 2, 1);
    cheeseTable[2][7] = new Cheese(CheeseColor::black, CheeseKind::pao, 2, 7);
    cheeseTable[3][0] = new Cheese(CheeseColor::black, CheeseKind::bing, 3, 0);
    cheeseTable[3][2] = new Cheese(CheeseColor::black, CheeseKind::bing, 3, 2);
    cheeseTable[3][4] = new Cheese(CheeseColor::black, CheeseKind::bing, 3, 4);
    cheeseTable[3][6] = new Cheese(CheeseColor::black, CheeseKind::bing, 3, 6);
    cheeseTable[3][8] = new Cheese(CheeseColor::black, CheeseKind::bing, 3, 8);

    // red
    cheeseTable[9][0] = new Cheese(CheeseColor::red, CheeseKind::che, 9, 0);
    cheeseTable[9][1] = new Cheese(CheeseColor::red, CheeseKind::ma, 9, 1);
    cheeseTable[9][2] = new Cheese(CheeseColor::red, CheeseKind::xiang, 9, 2);
    cheeseTable[9][3] = new Cheese(CheeseColor::red, CheeseKind::shi, 9, 3);
    cheeseTable[9][4] = new Cheese(CheeseColor::red, CheeseKind::shuai, 9, 4);
    cheeseTable[9][5] = new Cheese(CheeseColor::red, CheeseKind::shi, 9, 5);
    cheeseTable[9][6] = new Cheese(CheeseColor::red, CheeseKind::xiang, 9, 6);
    cheeseTable[9][7] = new Cheese(CheeseColor::red, CheeseKind::ma, 9, 7);
    cheeseTable[9][8] = new Cheese(CheeseColor::red, CheeseKind::che, 9, 8);
    cheeseTable[7][1] = new Cheese(CheeseColor::red, CheeseKind::pao, 7, 1);
    cheeseTable[7][7] = new Cheese(CheeseColor::red, CheeseKind::pao, 7, 7);
    cheeseTable[6][0] = new Cheese(CheeseColor::red, CheeseKind::bing, 6, 0);
    cheeseTable[6][2] = new Cheese(CheeseColor::red, CheeseKind::bing, 6, 2);
    cheeseTable[6][4] = new Cheese(CheeseColor::red, CheeseKind::bing, 6, 4);
    cheeseTable[6][6] = new Cheese(CheeseColor::red, CheeseKind::bing, 6, 6);
    cheeseTable[6][8] = new Cheese(CheeseColor::red, CheeseKind::bing, 6, 8);

    emit modelChanged(this->cheeseTable);
}

void CheeseModel::receiveMousePress(CheesePoint cheesePoint)
{
    assert(cheesePoint.row >= 0 && cheesePoint.row <= 9 && cheesePoint.column >= 0 && cheesePoint.column <= 8);
    if (this->cheeseChosenPoint.row != -1)         // There has been a chosen cheese
        if (cheeseNextPoint.contains(cheesePoint)) // Can go to cheesePoint
        {
            // Chosen cheese go to cheesePoint
            delete cheeseTable[cheesePoint.row][cheesePoint.column];
            cheeseTable[cheesePoint.row][cheesePoint.column] = cheeseTable[this->cheeseChosenPoint.row][this->cheeseChosenPoint.column];
            cheeseTable[this->cheeseChosenPoint.row][this->cheeseChosenPoint.column] = nullptr;
            emit modelChanged(this->cheeseChosenPoint, cheesePoint);
            // Clear chosen cheese
            this->clearChosenCheese();
        }
        else // Cannot go to cheesePoint
        {
            if (cheeseTable[cheesePoint.row][cheesePoint.column] != nullptr) // There is a cheese at cheesePoint
                this->chooseCheese(cheesePoint);
            else // There is not a cheese at cheesePoint
                this->clearChosenCheese();
        }
    else // There has not been a chosen cheese
    {
        if (cheeseTable[cheesePoint.row][cheesePoint.column] != nullptr) // There is a cheese at cheesePoint
            this->chooseCheese(cheesePoint);
    }
}

void CheeseModel::receiveMousePress()
{
    // cheesePoint not OK
    this->clearChosenCheese();
}

void CheeseModel::chooseCheese(const CheesePoint &cheesePoint)
{
    // Chose cheese at cheesePoint
    this->cheeseChosenPoint = cheesePoint;
    this->chooseCheeseNextPoint(cheesePoint);
}

void CheeseModel::clearChosenCheese()
{
    // Clear chosen cheese
    this->cheeseChosenPoint.row = this->cheeseChosenPoint.column = -1;
    this->cheeseNextPoint.clear();
}

void CheeseModel::chooseCheeseNextPoint(const CheesePoint &cheesePoint)
{
    // TODO: add rules
    for (int i = 0; i < 10; ++i)
        for (int j = 0; j < 9; ++j)
            this->cheeseNextPoint.insert(CheesePoint(i, j));
}
