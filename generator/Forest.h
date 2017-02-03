#pragma once
#include <vector>
#include "TreeLine.h"
using namespace std;

class Tree;
class Forest{
  vector< vector<Tree*> > tab;
  int n;
  int treeCount;
  int time;

public:
  Forest(int n);
  ~Forest();
  bool isTree(int x, int y);
  void removeTree(int x, int y);
  int getSize();
  int getNoTrees();
  // If plot is empty plant tree; returns plot value before planting
  Tree* plantTree(int x, int y, Tree* tree);
  Tree* getTree(int x, int y);
  Tree* getRandomTree();
  int calcTime(int timeMultiplier);
  void seedRandomTree();

  void saveForest(string filename);
  void printTrees();
};
