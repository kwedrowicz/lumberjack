#pragma once
#include <string>
class Tree{
public:
  int height;
  int width;
  int weight_unit;
  int value_unit;

  int x;
  int y;

  Tree(int h, int w, int wu, int vu);

  int getValue();
  int getWeight();

  void randomize();

  std::string getFileString(int x, int y);
  bool isHeavier(Tree* b);
  int calcDistance(Tree* b);
  void makeHeavierThan(Tree* b);
  void reduceByHalf();
};
