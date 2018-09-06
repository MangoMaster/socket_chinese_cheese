#include "cheesemodel.h"
#include "modedialog.h"
#include "ui_modedialog.h"
#include <QtWidgets/QInputDialog>
#include <cassert>
#include <QtCore/QDebug>

const CheesePoint CheeseModel::MA_POINT_DELTA[8] = {CheesePoint(-2, 1), CheesePoint(-1, 2), CheesePoint(1, 2), CheesePoint(2, 1), CheesePoint(2, -1), CheesePoint(1, -2), CheesePoint(-1, -2), CheesePoint(-2, -1)};
const CheesePoint CheeseModel::MA_BIE_POINT_DELTA[8] = {CheesePoint(-1, 0), CheesePoint(0, 1), CheesePoint(0, 1), CheesePoint(1, 0), CheesePoint(1, 0), CheesePoint(0, -1), CheesePoint(0, -1), CheesePoint(-1, 0)};
const CheesePoint CheeseModel::XIANG_POINT_DELTA[4] = {CheesePoint(-2, 2), CheesePoint(2, 2), CheesePoint(2, -2), CheesePoint(-2, -2)};
const CheesePoint CheeseModel::XIANG_BIE_POINT_DELTA[4] = {CheesePoint(-1, 1), CheesePoint(1, 1), CheesePoint(1, -1), CheesePoint(-1, -1)};
const CheesePoint CheeseModel::SHI_POINT_DELTA[4] = {CheesePoint(-1, 1), CheesePoint(1, 1), CheesePoint(1, -1), CheesePoint(-1, -1)};
const CheesePoint CheeseModel::SHUAI_POINT_DELTA[4] = {CheesePoint(-1, 0), CheesePoint(1, 0), CheesePoint(0, -1), CheesePoint(0, 1)};
const CheesePoint CheeseModel::BING_RED_POINT_DELTA[3] = {CheesePoint(-1, 0), CheesePoint(0, -1), CheesePoint(0, 1)};
const CheesePoint CheeseModel::BING_BLACK_POINT_DELTA[3] = {CheesePoint(1, 0), CheesePoint(0, -1), CheesePoint(0, 1)};

CheeseModel::CheeseModel(QObject *parent)
    : QObject(parent), cheeseChosenPoint(-1, -1)
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
    // set mode
    QStringList modes;
    modes << tr("单人模式") << tr("单机对战模式") << tr("联机对战模式");
    bool ok;
    this->mode = QInputDialog::getItem(nullptr, tr("对战模式"), tr("选择对战模式"), modes, 1, false, &ok);
    if (!ok)
        return;
    if (this->mode == tr("单机对战模式"))
    {
        ModeDialog dialog;
        dialog.ui->spinBox_2->setValue(127);
        dialog.ui->spinBox_2->setReadOnly(true);
        dialog.ui->spinBox_3->setValue(0);
        dialog.ui->spinBox_3->setReadOnly(true);
        dialog.ui->spinBox_4->setValue(0);
        dialog.ui->spinBox_4->setReadOnly(true);
        dialog.ui->spinBox_5->setValue(1);
        dialog.ui->spinBox_5->setReadOnly(true);
        if (dialog.exec() == QDialog::Accepted)
        {
            dialog.ui->spinBox->value();
        }
        else
            return;
    }
    else if (this->mode == tr("联机对战模式"))
    {
        ModeDialog dialog;
        dialog.ui->spinBox_2->setValue(192);
        dialog.ui->spinBox_3->setValue(168);
        dialog.ui->spinBox_4->setValue(0);
        dialog.ui->spinBox_5->setValue(1);
        if (dialog.exec() == QDialog::Accepted)
        {
            dialog.ui->spinBox->value();
        }
        else
            return;
    }

    // set gaming
    this->gaming = true;

    // set color
    this->myCheeseColor = CheeseColor::red;
    this->currentStepColor = CheeseColor::red;

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

    emit modelChanged(this->cheeseTable, this->myCheeseColor);
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
            // next step
            switch (this->currentStepColor)
            {
            case CheeseColor::red:
                this->currentStepColor = CheeseColor::black;
                break;
            case CheeseColor::black:
                this->currentStepColor = CheeseColor::red;
                break;
            }
            emit nextStep(this->currentStepColor);
            // Clear chosen cheese
            this->clearChosenCheese();
        }
        else // Cannot go to cheesePoint
        {
            if (cheeseTable[cheesePoint.row][cheesePoint.column] != nullptr)                           // There is a cheese at cheesePoint
                if (cheeseTable[cheesePoint.row][cheesePoint.column]->color == this->currentStepColor) // Proper cheese color at cheesePoint
                    this->chooseCheese(cheesePoint);
                else // Not proper cheese color at cheesePoint
                    this->clearChosenCheese();
            else // There is not a cheese at cheesePoint
                this->clearChosenCheese();
        }
    else // There has not been a chosen cheese
    {
        if (cheeseTable[cheesePoint.row][cheesePoint.column] != nullptr)                           // There is a cheese at cheesePoint
            if (cheeseTable[cheesePoint.row][cheesePoint.column]->color == this->currentStepColor) // Proper cheese color at cheesePoint
                this->chooseCheese(cheesePoint);
    }
}

void CheeseModel::receiveMousePress() // clear chosen point
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
    int row = cheesePoint.row;
    int column = cheesePoint.column;
    switch (cheeseTable[row][column]->kind)
    {
    case CheeseKind::che:
        for (int i = row - 1; i >= 0; --i) // upward
        {
            if (cheeseTable[i][column] != nullptr) // There is a cheese in the way
            {
                if (cheeseTable[i][column]->color != this->currentStepColor) // Can eat cheese in the way
                    this->cheeseNextPoint.insert(CheesePoint(i, column));
                break;
            }
            this->cheeseNextPoint.insert(CheesePoint(i, column));
        }
        for (int i = row + 1; i <= 9; ++i) // downward
        {
            if (cheeseTable[i][column] != nullptr) // There is a cheese in the way
            {
                if (cheeseTable[i][column]->color != this->currentStepColor) // Can eat cheese in the way
                    this->cheeseNextPoint.insert(CheesePoint(i, column));
                break;
            }
            this->cheeseNextPoint.insert(CheesePoint(i, column));
        }
        for (int j = column - 1; j >= 0; --j) // leftward
        {
            if (cheeseTable[row][j] != nullptr) // There is a cheese in the way
            {
                if (cheeseTable[row][j]->color != this->currentStepColor) // Can eat cheese in the way
                    this->cheeseNextPoint.insert(CheesePoint(row, j));
                break;
            }
            this->cheeseNextPoint.insert(CheesePoint(row, j));
        }
        for (int j = column + 1; j <= 8; ++j) // rightward
        {
            if (cheeseTable[row][j] != nullptr) // There is a cheese in the way
            {
                if (cheeseTable[row][j]->color != this->currentStepColor) // Can eat cheese in the way
                    this->cheeseNextPoint.insert(CheesePoint(row, j));
                break;
            }
            this->cheeseNextPoint.insert(CheesePoint(row, j));
        }
        break;
    case CheeseKind::ma:
        for (int i = 0; i < 8; ++i)
        {
            int newRow = row + MA_POINT_DELTA[i].row;
            int newColumn = column + MA_POINT_DELTA[i].column;
            if (newRow >= 0 && newRow <= 9 && newColumn >= 0 && newColumn <= 8)                                                   // inside border
                if (cheeseTable[newRow][newColumn] == nullptr || cheeseTable[newRow][newColumn]->color != this->currentStepColor) // Can go there
                    if (cheeseTable[row + MA_BIE_POINT_DELTA[i].row][column + MA_BIE_POINT_DELTA[i].column] == nullptr)           // 不别马腿
                        this->cheeseNextPoint.insert(CheesePoint(newRow, newColumn));
        }
        break;
    case CheeseKind::pao:
        for (int i = row - 1, inTheWay = 0; i >= 0; --i) // upward
            if (inTheWay == 0)
            {
                if (cheeseTable[i][column] != nullptr) // There is a cheese in the way
                {
                    inTheWay = 1;
                    continue;
                }
                this->cheeseNextPoint.insert(CheesePoint(i, column));
            }
            else
            {
                if (cheeseTable[i][column] != nullptr) // There is a cheese in the way
                {
                    if (cheeseTable[i][column]->color != this->currentStepColor) // Can eat cheese in the way
                        this->cheeseNextPoint.insert(CheesePoint(i, column));
                    break;
                }
            }
        for (int i = row + 1, inTheWay = 0; i <= 9; ++i) // downward
            if (inTheWay == 0)
            {
                if (cheeseTable[i][column] != nullptr) // There is a cheese in the way
                {
                    inTheWay = 1;
                    continue;
                }
                this->cheeseNextPoint.insert(CheesePoint(i, column));
            }
            else
            {
                if (cheeseTable[i][column] != nullptr) // There is a cheese in the way
                {
                    if (cheeseTable[i][column]->color != this->currentStepColor) // Can eat cheese in the way
                        this->cheeseNextPoint.insert(CheesePoint(i, column));
                    break;
                }
            }
        for (int j = column - 1, inTheWay = 0; j >= 0; --j) // leftward
            if (inTheWay == 0)
            {
                if (cheeseTable[row][j] != nullptr) // There is a cheese in the way
                {
                    inTheWay = 1;
                    continue;
                }
                this->cheeseNextPoint.insert(CheesePoint(row, j));
            }
            else
            {
                if (cheeseTable[row][j] != nullptr) // There is a cheese in the way
                {
                    if (cheeseTable[row][j]->color != this->currentStepColor) // Can eat cheese in the way
                        this->cheeseNextPoint.insert(CheesePoint(row, j));
                    break;
                }
            }
        for (int j = column + 1, inTheWay = 0; j <= 8; ++j) // rightward
            if (inTheWay == 0)
            {
                if (cheeseTable[row][j] != nullptr) // There is a cheese in the way
                {
                    inTheWay = 1;
                    continue;
                }
                this->cheeseNextPoint.insert(CheesePoint(row, j));
            }
            else
            {
                if (cheeseTable[row][j] != nullptr) // There is a cheese in the way
                {
                    if (cheeseTable[row][j]->color != this->currentStepColor) // Can eat cheese in the way
                        this->cheeseNextPoint.insert(CheesePoint(row, j));
                    break;
                }
            }
        break;
    case CheeseKind::xiang:
        switch (this->currentStepColor)
        {
        case CheeseColor::red:
            for (int i = 0; i < 4; ++i)
            {
                int newRow = row + XIANG_POINT_DELTA[i].row;
                int newColumn = column + XIANG_POINT_DELTA[i].column;
                if (newRow >= 5 && newRow <= 9 && newColumn >= 0 && newColumn <= 8)                                                   // inside red xiang border
                    if (cheeseTable[newRow][newColumn] == nullptr || cheeseTable[newRow][newColumn]->color != this->currentStepColor) // Can go there
                        if (cheeseTable[row + XIANG_BIE_POINT_DELTA[i].row][column + XIANG_BIE_POINT_DELTA[i].column] == nullptr)     // 不塞象眼
                            this->cheeseNextPoint.insert(CheesePoint(newRow, newColumn));
            }
            break;
        case CheeseColor::black:
            for (int i = 0; i < 4; ++i)
            {
                int newRow = row + XIANG_POINT_DELTA[i].row;
                int newColumn = column + XIANG_POINT_DELTA[i].column;
                if (newRow >= 0 && newRow <= 4 && newColumn >= 0 && newColumn <= 8)                                                   // inside black xiang border
                    if (cheeseTable[newRow][newColumn] == nullptr || cheeseTable[newRow][newColumn]->color != this->currentStepColor) // Can go there
                        if (cheeseTable[row + XIANG_BIE_POINT_DELTA[i].row][column + XIANG_BIE_POINT_DELTA[i].column] == nullptr)     // 不塞象眼
                            this->cheeseNextPoint.insert(CheesePoint(newRow, newColumn));
            }
            break;
        }
        break;
    case CheeseKind::shi:
        switch (this->currentStepColor)
        {
        case CheeseColor::red:
            for (int i = 0; i < 4; ++i)
            {
                int newRow = row + SHI_POINT_DELTA[i].row;
                int newColumn = column + SHI_POINT_DELTA[i].column;
                if (newRow >= 7 && newRow <= 9 && newColumn >= 3 && newColumn <= 5)                                                   // inside red shi border
                    if (cheeseTable[newRow][newColumn] == nullptr || cheeseTable[newRow][newColumn]->color != this->currentStepColor) // Can go there
                        this->cheeseNextPoint.insert(CheesePoint(newRow, newColumn));
            }
            break;
        case CheeseColor::black:
            for (int i = 0; i < 4; ++i)
            {
                int newRow = row + SHI_POINT_DELTA[i].row;
                int newColumn = column + SHI_POINT_DELTA[i].column;
                if (newRow >= 0 && newRow <= 2 && newColumn >= 3 && newColumn <= 5)                                                   // inside black shi border
                    if (cheeseTable[newRow][newColumn] == nullptr || cheeseTable[newRow][newColumn]->color != this->currentStepColor) // Can go there
                        this->cheeseNextPoint.insert(CheesePoint(newRow, newColumn));
            }
            break;
        }
        break;
    // TODO: add shuai conflict
    case CheeseKind::shuai:
        switch (this->currentStepColor)
        {
        case CheeseColor::red:
            for (int i = 0; i < 4; ++i)
            {
                int newRow = row + SHUAI_POINT_DELTA[i].row;
                int newColumn = column + SHUAI_POINT_DELTA[i].column;
                if (newRow >= 7 && newRow <= 9 && newColumn >= 3 && newColumn <= 5)                                                   // inside red shuai border
                    if (cheeseTable[newRow][newColumn] == nullptr || cheeseTable[newRow][newColumn]->color != this->currentStepColor) // Can go there
                        this->cheeseNextPoint.insert(CheesePoint(newRow, newColumn));
            }
            break;
        case CheeseColor::black:
            for (int i = 0; i < 4; ++i)
            {
                int newRow = row + SHUAI_POINT_DELTA[i].row;
                int newColumn = column + SHUAI_POINT_DELTA[i].column;
                if (newRow >= 0 && newRow <= 2 && newColumn >= 3 && newColumn <= 5)                                                   // inside black shuai border
                    if (cheeseTable[newRow][newColumn] == nullptr || cheeseTable[newRow][newColumn]->color != this->currentStepColor) // Can go there
                        this->cheeseNextPoint.insert(CheesePoint(newRow, newColumn));
            }
            break;
        }
        break;
    case CheeseKind::bing:
        switch (this->currentStepColor)
        {
        case CheeseColor::red:
            if (cheesePoint.row >= 5) // 未过河
            {
                int newRow = cheesePoint.row - 1;
                if (cheeseTable[newRow][cheesePoint.column] == nullptr || cheeseTable[newRow][cheesePoint.column]->color != this->currentStepColor) // Can go there
                    this->cheeseNextPoint.insert(CheesePoint(newRow, cheesePoint.column));
            }
            else // 已过河
            {
                for (int i = 0; i < 3; ++i)
                {
                    int newRow = row + BING_RED_POINT_DELTA[i].row;
                    int newColumn = column + BING_RED_POINT_DELTA[i].column;
                    if (newRow >= 0 && newColumn >= 0 && newColumn <= 8)                                                                  // inside border
                        if (cheeseTable[newRow][newColumn] == nullptr || cheeseTable[newRow][newColumn]->color != this->currentStepColor) // Can go there
                            this->cheeseNextPoint.insert(CheesePoint(newRow, newColumn));
                }
            }
            break;
        case CheeseColor::black:
            if (cheesePoint.row <= 4) // 未过河
            {
                int newRow = cheesePoint.row + 1;
                if (cheeseTable[newRow][cheesePoint.column] == nullptr || cheeseTable[newRow][cheesePoint.column]->color != this->currentStepColor) // Can go there
                    this->cheeseNextPoint.insert(CheesePoint(newRow, cheesePoint.column));
            }
            else // 已过河
            {
                for (int i = 0; i < 3; ++i)
                {
                    int newRow = row + BING_BLACK_POINT_DELTA[i].row;
                    int newColumn = column + BING_BLACK_POINT_DELTA[i].column;
                    if (newRow <= 9 && newColumn >= 0 && newColumn <= 8)                                                                  // inside border
                        if (cheeseTable[newRow][newColumn] == nullptr || cheeseTable[newRow][newColumn]->color != this->currentStepColor) // Can go there
                            this->cheeseNextPoint.insert(CheesePoint(newRow, newColumn));
                }
            }
            break;
        }
        break;
    }
}
