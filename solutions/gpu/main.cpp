#include "KernelAPI.h"
#include <iostream>
#include <fstream>
#include <istream>
#include <string>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include "Structs.h"

#define MAXK 10000
#define MAXN 1000

#define TIME1 1.0
#define TIME2 10.0
#define TIME3 20.0

using namespace std;

string directions[] = { "down", "right", "up", "left" };

int timeLimit, mapSize, treeCount; //treeCount accounts for the fake tree at 0,0

int best = 0;
int bestLength = 0;

vector<Tree> trees;
//double matrix[MAXK+1][MAXK+1];

int calculate_distance(int i, int j)
{
	return abs(trees[i].x - trees[j].x) + abs(trees[i].y - trees[j].y);
}

int tree_value(int i)
{
	return trees[i].h * trees[i].d * trees[i].p;
}

int tree_weight(int i)
{
	return trees[i].h * trees[i].d * trees[i].c;
}

void calculateCutValues(int *treeCutValues, int i, int *forest, vector<bool> &cutted)
{
	if (i == 0 || cutted[i])
	{
/*		*treeCutValues = 0;
		*(treeCutValues + 1) = 0;*/
		treeCutValues[i * 2] = 0;
		treeCutValues[i * 2 + 1] = 0;
	}
	else
	{
		int maxsum = 0;
		int dir = 0;
		for (int j = 0; j < 4; j++)
		{
			int sum = tree_value(i);
			int height = trees[i].h;
			int x = trees[i].x;
			int y = trees[i].y;
			int cur_tree = i;
			int dx = 0;
			int	dy = 0;
			if (j == 0)
			{
				dy = -1;
			}
			if (j == 1)
			{
				dx = 1;
			}
			if (j == 2)
			{
				dy = 1;
			}
			if (j == 3)
			{
				dx = -1;
			}
			height--;
			while (height > 0)
			{
				x += dx;
				y += dy;
				if (x < 0 || y < 0 || x >= mapSize || y >= mapSize)
					break;
				if (forest[y * mapSize + x] > 0 && !cutted[forest[y * mapSize + x]])
				{
					if (tree_weight(cur_tree) > tree_weight(forest[y * mapSize + x]))
					{
						sum += tree_value(forest[y * mapSize + x]);
						height = trees[forest[y * mapSize + x]].h;
						cur_tree = forest[y * mapSize + x];
					}
					else
						break;
				}
				height--;
			}
			if(sum > maxsum)
			{
				maxsum = sum;
				dir = j;
			};
			//  cout << sum <<" ";
		}
		treeCutValues[i * 2] = maxsum;
		treeCutValues[i * 2 + 1] = dir;
		// cout <<endl;
	}
}

void updatePheromones(float *pheroms, int *scores, int *bestPath, int *lengths) {
	// Evaporation
	for (int i = 0; i < treeCount; i++)
	{
		for (int j = i+1; j < treeCount; j++)
		{
			float newval = pheroms[i*treeCount + j] * DELTA;
			if (newval < MIN_PHER) newval = MIN_PHER;
			pheroms[i*treeCount + j] = newval;
			pheroms[j*treeCount + i] = newval;
		}
	}
/*	for (int i = 0; i < bestLength; i++)
	{
		cout << bestPath[i * 2] << " ";
	}
	cout << endl;*/
	float newval = pheroms[bestPath[0]] + PHER_INCREASE;
	if (newval > MAX_PHER) newval = MAX_PHER;
	pheroms[bestPath[0] * treeCount] = newval;
	pheroms[bestPath[0]] = newval;
	for (int i = 1; i < bestLength; i++)
	{
		newval = pheroms[bestPath[i * 2] * treeCount + bestPath[i * 2 - 2]] + PHER_INCREASE;
		if (newval > MAX_PHER) newval = MAX_PHER;
		pheroms[bestPath[i * 2] * treeCount + bestPath[i * 2 - 2]] = newval;
		pheroms[bestPath[i * 2 - 2] * treeCount + bestPath[i * 2]] = newval;
	}
	//cout << " " << maxph << " " << minph << " ";
	// Add ant pheromones
	
	/*for (int ant = 0; ant < ANTS; ant++)
	{
		if (lengths[ant] > 0)
		{
			// update pheroms from/to fake tree 0
			float newval = pheroms[paths[0]] + ((float)scores[ant] / (float)best);
			if (newval > maxp) newval = maxp;
			pheroms[paths[0] * treeCount] = newval;
			pheroms[paths[0]] = newval;
		}
		for (int i = 1; i < lengths[ant]; i++)
		{	
			float newval = pheroms[paths[i*2] * treeCount + paths[i*2-2]] + ((float)scores[ant] / (float)best);
			if (newval > maxp) newval = maxp;
			pheroms[paths[i * 2] * treeCount + paths[i * 2 - 2]] = newval;
			pheroms[paths[i * 2-2] * treeCount + paths[i * 2]] = newval;
		}
	} */

/*	int count = 0;
	for (int i = 0; i < treeCount; i++)
	{
		for (int j = 0; j < treeCount; j++)
		{
			cout << pheroms[i*treeCount + j] << " ";
		}
		cout << endl;
	}
	cout << endl;*/
	//cout << " " << count << " ";
}

int* loadFromStream(istream &stream) {
	stream >> timeLimit >> mapSize >> treeCount;
	treeCount++; 
	trees.resize(treeCount);
	int *forest = new int[mapSize * mapSize];
	for (int i = 0; i < mapSize; i++)
	{
		for (int j = 0; j < mapSize; j++)
		{
			forest[i*mapSize + j] = 0;
		}
	}
	trees[0] = { 0, 0, 0, 0, 0, 0 };
	for (int i = 1; i < treeCount; i++)
	{
		int x, y, h, d, c, p;
		stream >> x >> y >> h >> d >> c >> p;
		forest[y * mapSize + x] = i; // assign tree index from trees vector to map field
		trees[i] = { x, y, h, d, c, p };
	}
/*	for (int i = 0; i < mapSize; i++)
	{
		for (int j = 0; j < mapSize; j++)
		{
			cout << forest[i*mapSize + j] << " ";
		}
		cout << endl;
	}*/
	return forest;
}

void calcGpuMatrices(int *dists, float *pheroms, int *cutValues, int *forest)
{
	vector<bool> cutted(treeCount, false);
	for (int i = 0; i < treeCount; i++)
	{
		for (int j = i; j < treeCount; j++)
		{
			if (i == j)
			{
				dists[i*treeCount + j] = 0;
				pheroms[i*treeCount + j] = 0;
			}
			else {
				int dist = calculate_distance(i, j);
				dists[i*treeCount + j] = dist;
				dists[j*treeCount + i] = dist;
				pheroms[i*treeCount + j] = MIN_PHER;
				pheroms[j*treeCount + i] = MIN_PHER;
			}
		}
		calculateCutValues(cutValues, i, forest, cutted);
	}
/*	for (int i = 0; i < treeCount ; i++)
	{
		cout << '[' << i << "] " << cutValues[i*2] << " " << cutValues[i*2+1] << endl;
	}*/
}

void resetPheromones(float *pheroms)
{

	for (int i = 0; i < treeCount; i++)
	{
		for (int j = i; j < treeCount; j++)
		{
			if (i == j)
			{
				pheroms[i*treeCount + j] = 0;
			}
			else {
				pheroms[i*treeCount + j] = MAX_PHER;
				pheroms[j*treeCount + i] = MAX_PHER;
			}
		}
	}
}

void printTime(clock_t begin, clock_t end, string label)
{
	cout << label << static_cast<float>(end - begin) / CLOCKS_PER_SEC << endl;
}


/*	for (int i = 0; i < treeCount; i++)
	{
		for (int j = 0; j < treeCount; j++)
		{
			cout << distances[i*treeCount + j] << " ";
		}
		cout << endl;
	}*/
int cutTree(int i, int direction, int *forest, vector<bool> &cutted)
{
	int treesCut = 1;
	int sum = tree_value(i);
	int height = trees[i].h;
	int x = trees[i].x;
	int y = trees[i].y;
	int cur_tree = i;
	cutted[cur_tree] = true;
	int dx, dy;
	if (direction == 0)
	{
		dx = 0;
		dy = -1;
	}
	if (direction == 1)
	{
		dx = 1;
		dy = 0;
	}
	if (direction == 2)
	{
		dx = 0;
		dy = 1;
	}
	if (direction == 3)
	{
		dx = -1;
		dy = 0;
	}
	height--;
	while (height > 0)
	{
		x += dx;
		y += dy;
		if (x < 0 || y < 0 || x >= mapSize || y >= mapSize)
			break;
		if (!cutted[forest[y * mapSize + x]] && forest[y * mapSize + x] > 0)
		{
			if (tree_weight(cur_tree) > tree_weight(forest[y * mapSize + x]))
			{
				sum += tree_value(forest[y * mapSize + x]);
				height = trees[forest[y * mapSize + x]].h;
				cur_tree = forest[y * mapSize + x];
				cutted[cur_tree] = true;
				treesCut++;
			}
			else
				break;
		}
		height--;
	}
	return sum;
}

void print_result(int *bestPath, int bestLength){
	ofstream file;
	file.open("result");
	
	
	//cout << "Czas do koñca: "<<best_ant.t<<"\n";
	/*for(int i = 1; i < best_ant.cutted.size(); i++){
	cout << best_ant.cutted[i] <<" ";
	}*/
	cout << endl;
	for (int i = 0; i < bestLength *2; i++)
	{
		cout << bestPath[i] << " ";
	}
	cout << endl;
	for (int i = 0; i < bestLength * 2; i+=2)
	{
		int horizontal;
		int vertical;
		if (i == 0)
		{
			horizontal = trees[bestPath[i]].x;
			vertical = trees[bestPath[i]].y;
		}
		else {
			horizontal = trees[bestPath[i]].x - trees[bestPath[i - 2]].x;
			vertical = trees[bestPath[i]].y - trees[bestPath[i - 2]].y;
		}
		while (horizontal > 0)
		{
			file << "move right\n";
			horizontal--;
		}
		while (horizontal < 0)
		{
			file << "move left\n";
			horizontal++;
		}
		while (vertical > 0)
		{
			file << "move up\n";
			vertical--;
		}
		while (vertical < 0)
		{
			file << "move down\n";
			vertical++;
		}
		file << "cut " << directions[bestPath[i + 1]] << "\n";
	}

	file.close();
}


void runGreedy(int *distances, float *pheromones, int *forest, int* bestPath)
{
	int currentTree = 0;
	int remainingTime = timeLimit;
	vector<int> chosenTrees;
	vector<int> directions;
	vector<bool> cutted(treeCount, false);
	int score = 0;
	int usedTime = 0;
	chosenTrees.push_back(0);
	directions.push_back(0);

	vector<int> cutValues(treeCount * 2);
	
/*	for (int a = 1; a < treeCount; a++)
	{
		cutted[a] = false;
	}*/
	cutted[0] = true;
	cutValues[0] = 0;
	cutValues[1] = 0;
	while (remainingTime > 0)
	{
		for (int a = 1; a < treeCount; a++)
		{
			calculateCutValues(&cutValues[0], a, forest, cutted);
		}
		
		double best_value = -1;
		int best_tree = -1;
		int cut_direction = -1;
		for (int j = 1; j < treeCount; j++)
		{
			int cost = trees[j].d + distances[currentTree * treeCount + j];
			int direction = cutValues[j * 2 + 1];
			if (!cutted[j] && remainingTime >= cost)
			{
				double res = cutValues[j*2] / (double)cost;
				if (res > best_value)
				{
					best_value = res;
					best_tree = j;
					cut_direction = direction;
				}
			}
		}
		if (best_value < 0)
			break;
		chosenTrees.push_back(best_tree);
		directions.push_back(cut_direction);
		int cost = trees[best_tree].d + distances[currentTree * treeCount + best_tree];
		remainingTime -= cost;
		score += cutTree(best_tree, cut_direction, forest, cutted);
		currentTree = best_tree;
		usedTime += cost;
		
	}

	best = score;
	cout << "GREEDY SCORE: " << best << endl;
	for (int i = 1; i < chosenTrees.size(); i++)
	{
		pheromones[(i - 1)*treeCount + i] += 1;
		pheromones[i*treeCount + i - 1] += 1;
	}
	bestLength = chosenTrees.size() - 1;
	for (int i = 0; i < bestLength; i++)
	{
		bestPath[i*2] = chosenTrees[i + 1];
		bestPath[i*2 + 1] = directions[i+1];
	}
}


int main(int argc, char** argv)
{
	int *forest;
	const clock_t beginTime = clock();
	float realtimeLimit;

	if (argc == 2) {
		// load from file
		ifstream file;
		file.open(argv[1]);
		if (!file.is_open())
			return 100;
		forest = loadFromStream(file);
		file.close();
	}
	else {
		// load from console
		//cout << "Input forest description:" << endl;
		forest = loadFromStream(cin);
	}
	
	if (treeCount-1 <= 100)
		realtimeLimit = TIME1;
	else if (treeCount-1 <= 1000)
		realtimeLimit = TIME2;
	else
		realtimeLimit = TIME3;

	int *resultScores = new int[ANTS];
	int *resultLengths = new int[ANTS];
	int *resultPaths = new int[ANTS * treeCount];
	int *bestPath = new int[treeCount];

	vector<int> distances(treeCount * treeCount);
	vector<float> pheromones(treeCount * treeCount);
	//int *distances = new int[treeCount*treeCount];
	vector<int> cutValues(treeCount * 2); // store best value + direction in pairs
	calcGpuMatrices(&distances[0], &pheromones[0], &cutValues[0], forest);
	runGreedy(&distances[0], &pheromones[0], forest, bestPath);
	//return 1;
	clock_t initgpuTime = clock();
	initGPU(treeCount, mapSize, &trees[0], &distances[0], &cutValues[0], forest);
	printTime(initgpuTime, clock(), "INIT GPU TIME: ");



	clock_t avgIterTime = 0;
	int iters = 0;
	int stagnantIters = 0;
	while ( (clock() - beginTime + avgIterTime*2) / CLOCKS_PER_SEC < realtimeLimit) {
		iters++;
		clock_t iterTime = clock();
		runKernel(treeCount, &pheromones[0], timeLimit, mapSize, resultScores, resultPaths, resultLengths);
		
		for (int i = 0; i < ANTS; i++)
		{
			//cout << resultScores[i] << "/" << resultPaths[i] << "/" << resultPaths[i+1] << "  ";
			if (resultScores[i] > best)
			{
				best = resultScores[i];
				bestLength = resultLengths[i];
				memcpy(bestPath, resultPaths + i * treeCount, sizeof(int)*bestLength*2);
				cout << "BEST LENGTH: " << bestLength;
				cout << "   NEW BEST ANT: " << resultScores[i] << endl;
				stagnantIters = 0;
/*				for (int j = 0; j < bestLength; j++)
				{
					cout << bestPath[j*2] << " DIR:" << bestPath[j*2+1] << " ";
				}	*/	
			}

		}

		if (stagnantIters > STAGNANT_ITERATIONS)
		{
		//	resetPheromones(&pheromones[0]);
			cout << "Reset after stagnant" << endl;
			stagnantIters = 0;
		}
		else {
			stagnantIters++;
			updatePheromones(&pheromones[0], resultScores, bestPath, resultLengths);
		}
		//cout << clock() - iterTime << endl;
		if (avgIterTime != 0)
		{
			avgIterTime = (avgIterTime + clock() - iterTime) /2;
		} else
		{
			avgIterTime = clock() - iterTime;
		}
	}
	cleanupGPU();

	delete[] resultPaths;
	delete[] resultScores;
	delete[] resultLengths;
	delete[] forest;
	delete[] bestPath;
	printTime(0, avgIterTime, "ITERATION TIME: ");
	printTime(beginTime, clock(), "TOTAL TIME EVALUATED: ");
	cout << "ITERATIONS: " << iters << endl;
	cout << "BEST RESULT: " << best << " LEN: " << bestLength << endl;
	//print_result(bestPath, bestLength);
	return 0;
}