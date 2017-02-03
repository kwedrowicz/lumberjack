#pragma once
#include <vector>

#define ANTS 200
#define THREADS_PER_ANT 128 // multiple of 32

#define MAX_TREES 10000

#define ALPHA 1 // pheromone power
#define BETA 1 // distance
#define GAMMA 1 // tree value
#define DELTA 0.7 // pheromone evaporation

const double MIN_PHER = 1;
const double MAX_PHER = 1000;
const int STAGNANT_ITERATIONS = 50;
const double PHER_INCREASE = 100;

using namespace std;

struct Tree{
	// x,y - position on map
	// h - height
	// d - diameter
	// c - weight per volume unit
	// p - price/value per volume unit
	// volume = h * d
	int x, y, h, d, c, p;
};

/*struct Ant{
	// i - index of current tree the ant is in
	// t - time left 
	// res - result
	// total - distance travelled
	int i, t, res, total;
	vector<int> cutted;
	vector<int> directions;
};*/