#include "TreeLine.h"
#include "Forest.h"
#include "Tree.h"
#include "Constants.h"
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <algorithm>


TreeLine::TreeLine(Forest *forest, Position start, Position end) : forest(forest), start(start), end(end) 
{
	/*
	if (this->end.x < this->start.x)
	{
		int temp = this->end.x;
		this->end.x = this->start.x;
		this->start.x = temp;
	}
	if (this->end.y < this->start.y)
	{
		int temp = this->end.y;
		this->end.y = this->start.y;
		this->start.y = temp;
	}*/
}

Position TreeLine::determineOrientFromIntersection(const set<Tree*>& intTrees)
{
	length = max(abs(start.x-end.x), abs(start.y-end.y))+1;
	Position increment(0, 0);
	auto it = intTrees.begin();
	Tree *tree1 = *it;
	advance(it, 1);
	Tree *tree2 = *it;

	if (tree1->x != tree2->x )
	{
		if (tree1->x < tree2->x)
		{
			Tree *temp = tree2;
			tree2 = tree1;
			tree1 = temp;
		}
		if (tree1->isHeavier(tree2))
		{
			orient = Orientation::LEFT;
			increment.x = -1;
			if (start.x < end.x)
			{
				int temp = start.x;
				start.x = end.x;
				end.x = temp;
			}
		}
		else {
			orient = Orientation::RIGHT;
			increment.x = 1;
			if (start.x > end.x)
			{
				int temp = start.x;
				start.x = end.x;
				end.x = temp;
			}
		}
	}

	if (tree1->y != tree2->y)
	{
		if (tree1->y < tree2->y)
		{
			Tree *temp = tree1;
			tree1 = tree2;
			tree2 = tree1;
		}
		if (tree1->isHeavier(tree2))
		{
			orient = Orientation::DOWN;
			increment.y = -1;
			if (start.y < end.y)
			{
				int temp = start.y;
				start.y = end.y;
				end.y = temp;
			}
		}
		else {
			orient = Orientation::UP;
			increment.y = 1;
			if (start.y > end.y)
			{
				int temp = start.y;
				start.y = end.y;
				end.y = temp;
			}
		}
	}

	return increment;
}

Position TreeLine::determineOrientation()
 {
	length = start.x - end.x;
	Position increment(0,0);
	if(length > 0) {
		orient = Orientation::LEFT;
		increment.x = -1;
	} else if(length < 0) {
		orient = Orientation::RIGHT;
		increment.x = 1;
	} else {
		length = start.y - end.y;
		if(length > 0) {
			orient = Orientation::DOWN;
			increment.y = -1;
		} else if(length < 0) {
			orient = Orientation::UP;
			increment.y = 1;
		} else
			assert(false);
	}
	if(length < 0)
		length = -length;
	length++;
	length = max(abs(start.x - end.x), abs(start.y - end.y))+1;
	return increment;
}

vector<Tree*>& TreeLine::getTrees() {
	return trees;
}

void TreeLine::generateLine(int noTrees, const set<Tree*> &intersectionTrees)
 {	 
	 Position increment(0,1);
	 if (intersectionTrees.size() < 2)
		 increment = determineOrientation();
	 else
		 increment = determineOrientFromIntersection(intersectionTrees);
	Position currPos = start;
	if (length < noTrees)
	{
		if (length < intersectionTrees.size())
		{
			assert(false);
		}
		noTrees = intersectionTrees.size();
	}
	//assert(length >= noTrees);
	int lenCopy = length;
	int intersectionTreesLeft = intersectionTrees.size();
	while(lenCopy-- && noTrees)
	{
		bool plant = false;
		if(noTrees == lenCopy)
			plant = true;
		else if( rand()%(lenCopy-noTrees) == 0)
			plant = true;

		bool currentlyOnIntersectionTree = false;
		for (Tree* tree : intersectionTrees)
		{
			if (tree->x == currPos.x && tree->y == currPos.y)
			{
				currentlyOnIntersectionTree = true;
			}
		}
		if (noTrees == intersectionTreesLeft && noTrees < lenCopy && !currentlyOnIntersectionTree)
		{
			plant = false;
		}
		if(plant)
		{
			if (currentlyOnIntersectionTree)
				intersectionTreesLeft--;
			Tree *tree = new Tree(rand()%Constants::MAX_HEIGHT+1, rand()%Constants::MAX_WIDTH+1, rand()%Constants::MAX_WEIGHT+1, rand()%Constants::MAX_VALUE+1);
			//tree->reduceByHalf();
			Tree *prev = forest->plantTree(currPos.x, currPos.y, tree);
			if(prev != nullptr)
			{
				// The plot was already taken
				trees.push_back(prev);
			}
			else {
				trees.push_back(tree);
				tree->x = currPos.x;
				tree->y = currPos.y;
			}
			treePos.push_back(Position(currPos));
			noTrees--;
		}
		currPos = currPos + increment;
	}
	fixLine();
}

void TreeLine::fixLine()
{
	// Make sure every tree can be reached by one of the previous trees

	for (int i = trees.size() - 1; i > 0; i--)
	{
		int lastReached = -1;
		for (int j = i - 1; j >= 0; j--)
		{
			if (trees[i]->calcDistance(trees[j]) < trees[j]->height)
			{
				lastReached = j;
				break;
			}
		}
		// If none reach set closest to reach
		if (lastReached == -1)
		{
			trees[i - 1]->height = trees[i - 1]->calcDistance(trees[i]) + 1;
			trees[i - 1]->makeHeavierThan(trees[i]);
		}
		else
		{
			trees[lastReached]->makeHeavierThan(trees[i]);
		}
	}
}

int TreeLine::getValue() {
	int sum = 0;
	for(auto tree : trees)
		sum += tree->getValue();
	return sum;
}

Position TreeLine::getStartingPoint() {
	return start;
}  
