#ifndef LUMBERJACK_TREE_H
#define LUMBERJACK_TREE_H


class Tree{
public:
    int height;
    int width;
    int weight_unit;
    int value_unit;
    int x;
    int y;
    int number;
    bool isCut = false;

    Tree(int h, int w, int wu, int vu, int x = 0, int y = 0, int num = 0);
    Tree(const Tree &tree);

    int getValue();

    int getWeight();

    bool isHeavier(Tree b);

    int getDistance(Tree j);
};


#endif //LUMBERJACK_TREE_H
