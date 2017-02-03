#include "ForestGenerator.h"
#include "Crossing.h"
#include "TreeLine.h"
#include "Forest.h"
#include "Tree.h"
#include "Constants.h"
#include <algorithm>
#include <cassert>
#include <cstdio>
#include <iostream>
using namespace std;

struct Rectangle
{
	Rectangle(int minX, int minY, int maxX, int maxY) : minX(minX), minY(minY), maxX(maxX), maxY(maxY) {}
	int minX;
	int minY;
	int maxX;
	int maxY;
};

ForestGenerator::ForestGenerator()
{
}


ForestGenerator::~ForestGenerator()
{
}

void ForestGenerator::clearCrossings()
{
	for (unsigned int i = 0; i < crossings.size(); ++i)
	{
		delete crossings[i];
	}
	crossings.clear();
}

Forest* ForestGenerator::generateForest(int size, int noCrossings, bool genNoise, bool randLines, int percentLimitTrees)
{
	clearCrossings();
	forest = new Forest(size);
	int nCrossings = noCrossings;
	vector<Rectangle> dividedThisRound;
	vector<Rectangle> toDivideThisRound;
	toDivideThisRound.push_back(Rectangle(0, 0, size - 1, size - 1));
	bool divideY = static_cast<bool>(rand()%2);
	// Divide space into rectangles
	while (--nCrossings)
	{
		Rectangle rect = toDivideThisRound.back();	
		if (divideY)
		{
			int ySpan = rect.maxY - rect.minY;
			// dont divite if it's too small already
			if (ySpan < 5)
			{
				dividedThisRound.push_back(Rectangle(rect));
				nCrossings++;
			}
			else {
				int newY = ((rect.minY + rect.maxY) / 2 - ySpan / 5) + rand() % (ySpan * 2 / 5);
				dividedThisRound.push_back(Rectangle(rect.minX, rect.minY, rect.maxX, newY));
				dividedThisRound.push_back(Rectangle(rect.minX, newY, rect.maxX, rect.maxY));
			}
		}
		else
		{
			int xSpan = rect.maxX - rect.minX;
			if (xSpan < 5)
			{
				dividedThisRound.push_back(Rectangle(rect));
				nCrossings++;
			}
			else {
				int newX = ((rect.minX + rect.maxX) / 2 - xSpan/5) + rand() % (xSpan * 2 / 5);
				dividedThisRound.push_back(Rectangle(rect.minX, rect.minY, newX, rect.maxY));
				dividedThisRound.push_back(Rectangle(newX, rect.minY, rect.maxX, rect.maxY));
			}
		}
		toDivideThisRound.pop_back();
		if (toDivideThisRound.empty())
		{
			toDivideThisRound.swap(dividedThisRound);
			divideY = !divideY;
		}
	}
	assert(dividedThisRound.size() + toDivideThisRound.size() == noCrossings);
	vector<Rectangle> rectanglesForCrossings;
	rectanglesForCrossings.insert(rectanglesForCrossings.begin(), dividedThisRound.begin(), dividedThisRound.end());
	rectanglesForCrossings.insert(rectanglesForCrossings.begin(), toDivideThisRound.begin(), toDivideThisRound.end());
	assert(rectanglesForCrossings.size() == noCrossings);
	
	vector<bool> alreadyGenerated(rectanglesForCrossings.size(), false);
	int currentRectangleToGenerate = rand() % rectanglesForCrossings.size();
	cout << currentRectangleToGenerate << " ";
	const float maxMultiplier = 0.7f + ((rand() % 31) / 100.0f);
	while (forest->getNoTrees() < maxMultiplier * Constants::MAX_NO_TREES)
	{
		while (alreadyGenerated[currentRectangleToGenerate])
		{
			currentRectangleToGenerate = rand() % rectanglesForCrossings.size();
			cout << currentRectangleToGenerate << " ";
		}
		Rectangle& rect = rectanglesForCrossings[currentRectangleToGenerate];
		crossings.push_back(new Crossing(forest, Position(rect.minX, rect.minY), Position(rect.maxX, rect.maxY)));
		int space = rect.maxX + rect.maxY - rect.minX - rect.minY;
		int noLines = space / 4 + rand() % (space / 4);
		crossings.back()->betterGenerateCrossing(noLines);
		alreadyGenerated[currentRectangleToGenerate] = true;
		cout << forest->getNoTrees() << endl;
	}
	cout << endl;
	cout << forest->getNoTrees() << " " << Constants::MAX_NO_TREES << endl;

/*	for (auto& rect : rectanglesForCrossings)
	{
		if (percentLimitTrees != 0 && percentLimitTrees < 35)
		{
			if (rand() % 100 > 100 * percentLimitTrees / 35)
				continue;
		}
		crossings.push_back(new Crossing(forest, Position(rect.minX, rect.minY), Position(rect.maxX, rect.maxY)));
		int space = rect.maxX + rect.maxY - rect.minX - rect.minY;
		int noLines = space / 4 + rand() % (space / 4);
		crossings.back()->betterGenerateCrossing(noLines);
		//printf("%d %d %d %d \n", rect.minX, rect.minY, rect.maxX, rect.maxY);
	}*/

	if (genNoise && percentLimitTrees == 0)
	{
		generateNoise(size*size*5/100);
	}
	else if(genNoise)
	{
		//int canSeed = size*size*percentLimitTrees / 100 - forest->getNoTrees();
		int canSeed = Constants::MAX_NO_TREES - forest->getNoTrees();
		if (canSeed > 0)
		{
			generateNoise(canSeed);
		}
	}
	if (randLines)
	{
		randomizeLines(70);
	}
	removeExcessTrees();
	//Crossing cr(forest, Position(0,0), Position(size-1, size-1));
	//cr.betterGenerateCrossing(3);
	//crossings.push_back(&cr);
	//TreeLine tl(forest, Position(0, 0), Position(0, size - 1));
	//tl.generateLine(3);
	//randomizeLines(100);
	return forest;
}


void ForestGenerator::removeExcessTrees()
{
	while (forest->getNoTrees() > Constants::MAX_NO_TREES)
	{
		Tree* tree = forest->getRandomTree();
		forest->removeTree(tree->x, tree->y);
	}
}

void ForestGenerator::generateNoise(int noiseTrees)
{	
	for (int i = 0; i < noiseTrees; i++){
		forest->seedRandomTree();
	}
}

// Treeline automatically calls fixLine after generating, so this function is redundant
void ForestGenerator::fixLines()
{
	for (unsigned int i = 0; i < crossings.size(); i++)
	{
		for (unsigned int j = 0; j < crossings[i]->getTreeLines().size(); j++)
		{			
			crossings[i]->getTreeLines()[j]->fixLine();
		}
	}
}
void ForestGenerator::randomizeLines(int chance)
{
	for (unsigned int i = 0; i < crossings.size(); i++)
	{
		for (unsigned int j = 0; j < crossings[i]->getTreeLines().size(); j++)
		{
			if (rand() % 100 < chance)
			{
				Tree* tree = crossings[i]->getTreeLines()[j]->getTrees()[rand() % crossings[i]->getTreeLines()[j]->getTrees().size()];
				if (rand() % 2)
				{
					tree->randomize();
				}
				else
				{
					tree->height /= 2;
					tree->width /= 2;
					if (tree->height == 0)
						tree->height++;
					if (tree->width == 0)
						tree->width++;
				}
			}
		}
	}
}

