#include "Tree.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include "Constants.h"
using namespace std;
Tree::Tree(int h, int w, int wu, int vu){
	height = h;
	width = w;
	weight_unit = wu;
	value_unit = vu;
}

int Tree::getValue(){
	return height*width*value_unit;
}

int Tree::getWeight(){
	return height*width*weight_unit;
}
void Tree::reduceByHalf()
{
	if (height > 2)
		height /= 2;
	if (weight_unit > 2)
		weight_unit /= 2;
	if (width > 2)
		width /= 2;
}
bool Tree::isHeavier(Tree* b){
	return getWeight() > b->getWeight();
}
void Tree::makeHeavierThan(Tree* b)
{
	while (!isHeavier(b))
	{
		weight_unit++;
		if (weight_unit > Constants::MAX_WEIGHT) {
			width++;
			weight_unit = rand() % Constants::MAX_WEIGHT + 1;
			if (width > Constants::MAX_WIDTH)
			{
				height++;
				width = rand() % Constants::MAX_WIDTH + 1;
				weight_unit = rand() % Constants::MAX_WEIGHT + 1;
			}
		}
	}
	if (height > Constants::MAX_HEIGHT)
	{
		height = Constants::MAX_HEIGHT;
	}
}
int Tree::calcDistance(Tree* b)
{
	return max(abs(x - b->x), abs(y - b->y));
}
string Tree::getFileString(int x, int y) {
	stringstream ss;
	ss << x << " " << y << " " << height << " " << width << " " << weight_unit << " " << value_unit << endl;
	return ss.str();
}

void Tree::randomize()
{
	height = (rand() % Constants::MAX_HEIGHT)+1;
	value_unit = (rand() % Constants::MAX_VALUE) + 1;
	weight_unit = (rand() % Constants::MAX_WEIGHT / 3) + 1;
	width = (rand() % Constants::MAX_WIDTH / 2) + 1;
}