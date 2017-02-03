#pragma once
#include <vector>
#include <set>
#include "Position.h"

using namespace std;
class Forest;
class Tree;

enum class Orientation {LEFT, RIGHT, UP, DOWN};
class TreeLine {
	vector<Tree*> trees;
	vector<Position> treePos;
	Position start;
	Position end;
	Forest* forest;
	Orientation orient;
	int length;

	Position determineOrientation();
	Position determineOrientFromIntersection(const set<Tree*>& intTrees);

public:
	TreeLine(Forest *forest, Position start, Position end);
	
	void generateLine(int noTrees, const set<Tree*> &intersectionTrees = set<Tree*>());
	void fixLine();
	int getValue();
	Position getStartingPoint();
	vector<Tree*>& getTrees();
};	
