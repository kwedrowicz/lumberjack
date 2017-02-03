#pragma once
#include "Position.h"
#include "TreeLine.h"
#include <vector>
class Crossing
{
	Forest *forest;
	std::vector<TreeLine*> treeLines;
	Position spanBegin;
	Position spanEnd;

public:
	Crossing(Forest* forest, Position sB, Position sE);
	~Crossing();

	void generateCrossing(int numberOfLines);
	void betterGenerateCrossing(int numberOfLines);

	std::vector<TreeLine*>& getTreeLines() {
		return treeLines;
	}
};

