#include "Constants.h"
#include <algorithm>
const int Constants::TRUE_MAX_HEIGHT = 15;
const int Constants::TRUE_MAX_WIDTH = 20;
const int Constants::ABSOLUTE_MAX_NO_TREES = 10000;
int Constants::MAX_TIME = 5000;
int Constants::MAX_SIZE = 1000;
int Constants::MAX_HEIGHT = TRUE_MAX_HEIGHT;
int Constants::MAX_WIDTH = TRUE_MAX_WIDTH;
int Constants::MAX_VALUE = 30;
int Constants::MAX_WEIGHT = 100;
int Constants::MAX_NO_TREES = ABSOLUTE_MAX_NO_TREES;

void Constants::limitBySize(int size)
{
	MAX_WIDTH = std::min(size, TRUE_MAX_WIDTH);
	MAX_HEIGHT = std::min(size, TRUE_MAX_HEIGHT);
}
void Constants::limitNoTrees(int noTrees)
{
	MAX_NO_TREES = std::min(noTrees, ABSOLUTE_MAX_NO_TREES);
}
void Constants::restore()
{
	MAX_WIDTH = TRUE_MAX_WIDTH;
	MAX_HEIGHT = TRUE_MAX_HEIGHT;
	MAX_NO_TREES = ABSOLUTE_MAX_NO_TREES;
}