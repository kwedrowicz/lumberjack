#ifndef LUMBERJACK_FOREST_H
#define LUMBERJACK_FOREST_H


#include <vector>
using namespace std;

class Tree;
class Forest{
    vector< vector<Tree*> > tab;
    int n;
public:
    Forest(int n);
    ~Forest();
    bool isTree(int x, int y);
    void removeTree(int x, int y);
    int getSize();
    // If plot is empty plant tree; returns plot value before planting
    Tree* plantTree(int x, int y, Tree* tree);
    Tree* getTree(int x, int y);
    void seedRandomTree();

    void printTrees();
};


#endif //LUMBERJACK_FOREST_H
