#ifndef CHEESE_MODEL_H_
#define CHEESE_MODEL_H_

#include <QtCore/QObject>
#include <cheese.h>

class CheeseModel : public QObject
{
    Q_OBJECT

  public:
    CheeseModel(QObject *parent = nullptr);
    ~CheeseModel();

  signals:
    void modelChanged(const std::array<std::array<Cheese *, 9>, 10> &changedCheese);
    void modelChanged(std::vector<const Cheese *> changedCheese);

  public slots:
    void setNewModel();

  private:
    std::array<std::array<Cheese *, 9>, 10> cheeseTable;
};

#endif // CHEESE_MODEL_H_