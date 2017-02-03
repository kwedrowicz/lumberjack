#include "Crossing.h"
#include "Forest.h"
#include "Tree.h"
#include "Constants.h"
#include <cstdio>
#include <cassert>
#include <algorithm>
#include <set>
#include <map>

using namespace std;
Crossing::Crossing(Forest *forest, Position sB, Position sE) : spanBegin(sB), spanEnd(sE), forest(forest)
{
	if (spanEnd.y < spanBegin.y)
	{
		int temp = spanBegin.y;
		spanBegin.y = spanEnd.y;
		spanEnd.y = temp;
	}
	if (spanEnd.x < spanBegin.x)
	{
		int temp = spanBegin.x;
		spanBegin.x = spanEnd.x;
		spanEnd.x = temp;
	}
}


void Crossing::generateCrossing(int numberOfLines)
{
	int width = abs(spanBegin.x - spanEnd.x)+1;
	int height = abs(spanBegin.y - spanEnd.y)+1;
	assert(width+height > numberOfLines);
	int horizontalLines = 0;
	int verticalLines = 0;
	horizontalLines = rand() % height;
	if (horizontalLines > numberOfLines)
		horizontalLines = numberOfLines;
	if (numberOfLines - horizontalLines <= width)
	{
		verticalLines = numberOfLines - horizontalLines;
	}
	else
	{
		verticalLines = width;
		horizontalLines = numberOfLines - verticalLines;
	}



	for (int i = 0; horizontalLines != 0 && i < height; i++)
	{
		bool place = false;
		if (horizontalLines == height - i)
			place = true;
		else if (rand() % height-i)
			place = true;

		if (place)
		{
			horizontalLines--;
			treeLines.push_back(new TreeLine(forest, Position(spanBegin.x, spanBegin.y+i), Position(spanEnd.x, spanBegin.y+i)));
			treeLines.back()->generateLine(max(2,rand() % width));
		}
	}

	for (int i = 0; verticalLines != 0 && i < width; i++)
	{
		bool place = false;
		if (verticalLines == width - i)
			place = true;
		else if (rand() % width-i)
			place = true;

		if (place)
		{
			verticalLines--;
			treeLines.push_back(new TreeLine(forest, Position(spanBegin.x+i, spanBegin.y), Position(spanBegin.x+i, spanEnd.y)));
			treeLines.back()->generateLine(max(2,rand() % height));
		}
	}
	assert(treeLines.size() == numberOfLines);
}

void Crossing::betterGenerateCrossing(int numberOfLines)
{
	const int WIDTH = abs(spanBegin.x - spanEnd.x) + 1;
	const int HEIGHT = abs(spanBegin.y - spanEnd.y) + 1;
	assert(WIDTH + HEIGHT > numberOfLines);
	int horizontalLines = 0;
	int verticalLines = 0;
	int possibleLines = 0;

	set<Tree*> intersectionTrees;
	set<int> heights;
	set<int> widths;
	// Create intersection trees
	while (numberOfLines > possibleLines && forest->getNoTrees() < Constants::MAX_NO_TREES)
	{
		int x = spanBegin.x + rand() % WIDTH;
		int y = spanBegin.y + rand() % HEIGHT;
		Tree *tree = new Tree(1, 1, 1, 1);
		tree->randomize();
		tree->reduceByHalf();
		Tree *prev = forest->plantTree(x, y, tree);
		if (prev != nullptr)
		{
			prev->x = x;
			prev->y = y;
			intersectionTrees.insert(prev);
		}
		else
		{
			tree->x = x;
			tree->y = y;
			intersectionTrees.insert(tree);
		}
		if (widths.find(x) == widths.end())
		{
			possibleLines++;
			verticalLines++;
			widths.insert(x);
		}
		if (heights.find(y) == heights.end())
		{
			possibleLines++;
			horizontalLines++;
			heights.insert(y);
		}
	}

	for (auto width : widths)
	{
		set<Tree*> interTrees;
		for (auto tree : intersectionTrees)
		{
			if (tree->x == width)
			{
				interTrees.insert(tree);
			}
		}
		if (verticalLines && numberOfLines)
		{
			verticalLines--;
			numberOfLines--;
			if (rand()%2)
				treeLines.push_back(new TreeLine(forest, Position(width, spanBegin.y), Position(width, spanEnd.y)));
			else
				treeLines.push_back(new TreeLine(forest, Position(width, spanEnd.y), Position(width, spanBegin.y)));
			if (HEIGHT >= 4)
			{
				treeLines.back()->generateLine(max((int)interTrees.size() + 1, HEIGHT / 3 + rand() % (HEIGHT / 4)), interTrees);
			}
			else
			{
				treeLines.back()->generateLine((int)interTrees.size() + 1, interTrees);
			}
		}
	}

	for (auto height : heights)
	{
		set<Tree*> interTrees;
		for (auto tree : intersectionTrees)
		{
			if (tree->y == height)
			{
				interTrees.insert(tree);
			}
		}
		if (horizontalLines && numberOfLines)
		{
			horizontalLines--;
			numberOfLines--;
			if (rand() % 2)
				treeLines.push_back(new TreeLine(forest, Position(spanBegin.x, height), Position(spanEnd.x, height)));
			else
				treeLines.push_back(new TreeLine(forest, Position(spanEnd.x, height), Position(spanBegin.x, height)));
			if (WIDTH >= 4)
			{
				treeLines.back()->generateLine(max((int)interTrees.size() + 1, WIDTH / 3 + rand() % (WIDTH / 4)), interTrees);
			}
			else
			{
				treeLines.back()->generateLine((int)interTrees.size() + 1, interTrees);
			}
		}
	}

	//assert(treeLines.size() == numberOfLines);
}

Crossing::~Crossing()
{
	for (unsigned int i = 0; i < treeLines.size(); ++i)
		delete treeLines[i];
}
