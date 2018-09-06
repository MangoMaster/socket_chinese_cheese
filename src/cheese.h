#ifndef CHEESE_H_
#define CHEESE_H_

struct CheesePoint
{
    CheesePoint(int row, int column)
        : row(row), column(column) {}
    int row;
    int column;
};
inline bool operator==(CheesePoint first, CheesePoint second)
{
    return (first.row == second.row && first.column == second.column);
}
inline uint qHash(const CheesePoint &key, uint seed)
{
    return qHash(key.row, seed) ^ qHash(key.column, seed);
}

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
        : color(color), kind(kind), x(x), y(y), alive(alive) {}
    CheeseColor color;
    CheeseKind kind;
    int x;
    int y;
    bool alive;
};

#endif // CHEESE_H_