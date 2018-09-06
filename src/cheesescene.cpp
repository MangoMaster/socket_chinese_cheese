#include "cheesescene.h"
#include <QtWidgets/QGraphicsItem>
#include <QtWidgets/QGraphicsSceneMouseEvent>
#include <cmath>
#include <QtCore/QDebug>

CheeseScene::CheeseScene(CheeseColor color, QObject *parent)
    : QGraphicsScene(parent), color(color)
{
    QPixmap background = QPixmap(":/icons/background.png").scaled(800, 1200, Qt::KeepAspectRatio);
    this->addItem(new QGraphicsPixmapItem(background));
    for (int i = 0; i < 10; ++i)
        for (int j = 0; j < 9; ++j)
        {
            this->cheesePixmapTable[i][j] = new CheesePixmap();
            this->cheesePixmapTable[i][j]->setOffset(80 * (j + 1) - 75 / 2, 80 * (i + 1) - 75 / 2);
            this->addItem(cheesePixmapTable[i][j]);
        }
}

CheeseScene::~CheeseScene()
{
    for (auto &cheesePixmapArray : this->cheesePixmapTable)
        for (auto &cheesePixmap : cheesePixmapArray)
            delete cheesePixmap;
}

void CheeseScene::receiveModel(const std::array<std::array<Cheese *, 9>, 10> &changedCheese)
{
    for (int i = 0; i < 10; ++i)
        for (int j = 0; j < 9; ++j)
        {
            Cheese *cheese;
            // reverse scene
            switch (this->color)
            {
            case CheeseColor::red:
                cheese = changedCheese[i][j];
                break;
            case CheeseColor::black:
                cheese = changedCheese[9 - i][8 - j];
                break;
            }

            // no cheese
            if (cheese == nullptr)
            {
                if (!cheesePixmapTable[i][j]->pixmap().isNull())
                    this->cheesePixmapTable[i][j]->setPixmap(QPixmap());
                continue;
            }

            switch (cheese->color)
            {
            case CheeseColor::red:
                switch (cheese->kind)
                {
                case CheeseKind::che:
                    this->cheesePixmapTable[i][j]->setPixmap(QPixmap(":/icons/red_che.png"));
                    break;
                case CheeseKind::ma:
                    this->cheesePixmapTable[i][j]->setPixmap(QPixmap(":/icons/red_ma.png"));
                    break;
                case CheeseKind::pao:
                    this->cheesePixmapTable[i][j]->setPixmap(QPixmap(":/icons/red_pao.png"));
                    break;
                case CheeseKind::xiang:
                    this->cheesePixmapTable[i][j]->setPixmap(QPixmap(":/icons/red_xiang.png"));
                    break;
                case CheeseKind::shi:
                    this->cheesePixmapTable[i][j]->setPixmap(QPixmap(":/icons/red_shi.png"));
                    break;
                case CheeseKind::shuai:
                    this->cheesePixmapTable[i][j]->setPixmap(QPixmap(":/icons/red_shuai.png"));
                    break;
                case CheeseKind::bing:
                    this->cheesePixmapTable[i][j]->setPixmap(QPixmap(":/icons/red_bing.png"));
                    break;
                }
                break;
            case CheeseColor::black:
                switch (cheese->kind)
                {
                case CheeseKind::che:
                    this->cheesePixmapTable[i][j]->setPixmap(QPixmap(":/icons/black_che.png"));
                    break;
                case CheeseKind::ma:
                    this->cheesePixmapTable[i][j]->setPixmap(QPixmap(":/icons/black_ma.png"));
                    break;
                case CheeseKind::pao:
                    this->cheesePixmapTable[i][j]->setPixmap(QPixmap(":/icons/black_pao.png"));
                    break;
                case CheeseKind::xiang:
                    this->cheesePixmapTable[i][j]->setPixmap(QPixmap(":/icons/black_xiang.png"));
                    break;
                case CheeseKind::shi:
                    this->cheesePixmapTable[i][j]->setPixmap(QPixmap(":/icons/black_shi.png"));
                    break;
                case CheeseKind::shuai:
                    this->cheesePixmapTable[i][j]->setPixmap(QPixmap(":/icons/black_shuai.png"));
                    break;
                case CheeseKind::bing:
                    this->cheesePixmapTable[i][j]->setPixmap(QPixmap(":/icons/black_bing.png"));
                    break;
                }
                break;
            }
        }
}

void CheeseScene::receiveModel(CheesePoint startCheesePoint, CheesePoint endCheesePoint)
{
    cheesePixmapTable[endCheesePoint.row][endCheesePoint.column]->setPixmap(cheesePixmapTable[startCheesePoint.row][startCheesePoint.column]->pixmap());
    cheesePixmapTable[startCheesePoint.row][startCheesePoint.column]->setPixmap(QPixmap());
}

void CheeseScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        QPointF scenePoint = event->scenePos();
        if (scenePoint.x() >= 80 / 2 && scenePoint.x() < 80 / 2 + 9 * 80 && scenePoint.y() >= 80 / 2 && scenePoint.y() < 80 / 2 + 10 * 80)
        {
            int row = (scenePoint.y() - 80 / 2) / 80;
            int column = (scenePoint.x() - 80 / 2) / 80;
            emit mousePressed(CheesePoint(row, column));
        }
        else
            emit mousePressed();
    }
    QGraphicsScene::mousePressEvent(event);
}