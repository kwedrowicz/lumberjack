#pragma once
#include <vector>
class Forest;
class Crossing;
class ForestGenerator
{
	Forest* forest;
	std::vector<Crossing*> crossings;

	void clearCrossings();
	void removeExcessTrees();
public:
	ForestGenerator();
	~ForestGenerator();

	Forest* generateForest(int size, int noCrossings, bool genNoise, bool randLines, int percentLimitTrees = 0);
	void generateNoise(int noiseTrees);
	void randomizeLines(int chance);
	void fixLines();
};

