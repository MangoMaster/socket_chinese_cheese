#ifndef CHEESE_MODEL_H_
#define CHEESE_MODEL_H_

#include <QtCore/QObject>
#include <vector>
#include <array>
#include <memory>

enum CheeseColor
{
    red,
    black
};
enum CheeseKind
{
    che,
    ma,
    pao,
    xiang,
    shi,
    shuai,
    bing
};

struct Cheese
{
    Cheese(CheeseColor color, CheeseKind kind, uint8_t x, uint8_t y, bool alive = true);
    CheeseColor color;
    CheeseKind kind;
    uint8_t x;
    uint8_t y;
    bool alive;
};

class CheeseModel : public QObject
{
    Q_OBJECT

  public:
    CheeseModel(QObject *parent = nullptr);
    ~CheeseModel();

  signals:
    void modelChanged(std::array<std::array<Cheese *, 9>, 10> changedCheese);
    void modelChanged(std::vector<const Cheese *> changedCheese);

  public slots:
    void setNewModel();

  private:
    std::array<std::array<Cheese *, 9>, 10> cheeseTable;
};

#endif // CHEESE_MODEL_H_