#ifndef CHEESE_H_
#define CHEESE_H_

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
    Cheese(CheeseColor color, CheeseKind kind, int x, int y, bool alive = true)
        : color(color), kind(kind), x(x), y(y), alive(alive)
    {
    }
    CheeseColor color;
    CheeseKind kind;
    int x;
    int y;
    bool alive;
};

#endif // CHEESE_H_