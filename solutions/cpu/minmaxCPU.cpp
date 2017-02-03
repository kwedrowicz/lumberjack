/*
	Max - min ant system(MMAS)
	Added maximum and minimum pheromone amounts[tmax, tmin].Only global best or iteration best tour deposited pheromone <MAZ>.
	All edges are initialized to tmin and reinitialized to tmax when nearing stagnation.
*/

#include <iostream>
#include <fstream>
#include <istream>
#include <string>
#include <vector>
#include <cmath>
#include <ctime>
#include <algorithm>

#define MAXK 10000
#define MAXN 1000
#define ANTS 25
#define TIME1 0.95
#define TIME2 9.5
#define TIME3 58.0

using namespace std;

const double alfa = 1; // pheromone weight
const double beta = 1; // cost weight
const double gam = 1; // tree value weight
const double p = 0.7; // evap rate ( new = old * p )

const double MIN_PHER = 1;
const double MAX_PHER = 1000;
const int STAGNANT_ITERATIONS = 50;
const double PHER_INCREASE = 100;

string directions[] = { "down", "right", "up", "left" };

struct tree{
	// x,y - position on map
	// h - height
	// d - diameter
	// c - weight per volume unit
	// p - price/value per volume unit
	// volume = h * d
	void setValues(int ix, int iy, int ih, int id, int ic, int ip)
	{
		x = ix;
		y = iy;
		h = ih;
		d = id;
		c = ic;
		p = ip;
	}
	int x, y, h, d, c, p;
};

struct ant{
	// i - index of current tree the ant is in
	// t - time left 
	// res - result
	// total - distance travelled
	int i, t, res, total;
	vector<int> cutted;
	vector<int> directions;
};

vector<tree> trees;
int calculate_distance(int, int);
int timeLimit, mapSize, treeCount;

class Distances {
	int **matrix;
	int size;
public:
	Distances(int treeCount) {
		size = treeCount;
		matrix = new int*[treeCount - 1];
		for (int i = 0; i < treeCount - 1; i++) {
			matrix[i] = new int[treeCount - i - 1];
		}
		for (int i = 0; i < treeCount - 1; i++)
		{
			for (int j = 0; j < treeCount - i - 1; j++)
			{
				matrix[i][j] = calculate_distance(i, treeCount - j - 1);
			}
		}
	}
	~Distances() {
		for (int i = 0; i < size - 1; i++) {
			delete[] matrix[i];
		}
		delete[] matrix;
	}
	int get(int i, int j) {
		if (i < j) {
			return matrix[i][size - j - 1];
		}
		else {
			return matrix[j][size - i - 1];
		}
	}
};

class Pheromones {
	double **matrix;
	int size;
public:
	Pheromones(int treeCount) {
		size = treeCount;
		matrix = new double*[treeCount - 1];
		for (int i = 0; i < treeCount - 1; i++) {
			matrix[i] = new double[treeCount - i - 1];
		}
		for (int i = 0; i < treeCount - 1; i++)
		{
			for (int j = 0; j < treeCount - i - 1; j++)
			{
				matrix[i][j] = MIN_PHER;
			}
		}
	}
	Pheromones(int treeCount, ant bestAnt) {
		size = treeCount;
		matrix = new double*[treeCount - 1];
		for (int i = 0; i < treeCount - 1; i++) {
			matrix[i] = new double[treeCount - i - 1];
		}
		for (int i = 0; i < treeCount - 1; i++)
		{
			for (int j = 0; j < treeCount - i - 1; j++)
			{
				matrix[i][j] = MIN_PHER;
			}
		}
		for (size_t x = 1; x < bestAnt.cutted.size(); x++)
		{
			set(bestAnt.cutted[x - 1], bestAnt.cutted[x], get(bestAnt.cutted[x - 1], bestAnt.cutted[x]) + PHER_INCREASE);
		}
	}
	~Pheromones() {
		for (int i = 0; i < size - 1; i++) {
			delete[] matrix[i];
		}
		delete[] matrix;
	}
	void reset()
	{
		for (int i = 0; i < treeCount - 1; i++)
		{
			for (int j = 0; j < treeCount - i - 1; j++)
			{
				matrix[i][j] = MAX_PHER;
			}
		}
	}
	double get(int i, int j) {
		if (i < j) {
			return matrix[i][size - j - 1];
		}
		else {
			return matrix[j][size - i - 1];
		}
	}
	double *getPtr(int i, int j)
	{
		if (i < j) {
			return &matrix[i][size - j - 1];
		}
		else {
			return &matrix[j][size - i - 1];
		}
		
	}
	void set(int i, int j, double val) {
		if (i < j) {
			matrix[i][size - j - 1] = val;
		}
		else {
			matrix[j][size - i - 1] = val;
		}
	}
};



Distances *distances;
Pheromones *pheromones;
//double matrix[MAXK+1][MAXK+1];
int forest[MAXN][MAXN];
bool cutted[MAXK + 1];
int cut_value[MAXK + 1][4];
int greedyCutValues[MAXK + 1][4];
ant* ants[ANTS];
ant best_ant;
int best = 0;

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

int cutTree(int i, int direction)
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
		if (forest[x][y] > 0 && !cutted[forest[x][y]])
		{
			if (tree_weight(cur_tree) > tree_weight(forest[x][y]))
			{
				sum += tree_value(forest[x][y]);
				height = trees[forest[x][y]].h;
				cur_tree = forest[x][y];
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
void calculate_cut_values(int cut_value[][4])
{
	for (int i = 1; i <= treeCount; i++)
	{
		if (cutted[i]){
			for (int j = 0; j < 4; j++){
				cut_value[i][j] = 0;
			}
		}
		else {
			for (int j = 0; j < 4; j++) {
				int sum = tree_value(i);
				int height = trees[i].h;
				int x = trees[i].x;
				int y = trees[i].y;
				int cur_tree = i;
				int dx = 0;
				int dy = 0;
				if (j == 0) {
					dy = -1;
				}
				if (j == 1) {
					dx = 1;
				}
				if (j == 2) {
					dy = 1;
				}
				if (j == 3) {
					dx = -1;
				}
				height--;
				while (height > 0) {
					x += dx;
					y += dy;
					if (x < 0 || y < 0 || x >= mapSize || y >= mapSize)
						break;
					if (forest[x][y] > 0 && !cutted[forest[x][y]]) {
						if (tree_weight(cur_tree) > tree_weight(forest[x][y])) {
							sum += tree_value(forest[x][y]);
							height = trees[forest[x][y]].h;
							cur_tree = forest[x][y];
						}
						else
							break;
					}
					height--;
				}
				cut_value[i][j] = sum;
			}
		}
	}
}

void updatePheromones(ant *iterBestAnt) {
	for (size_t i = 0; i < trees.size(); i++)
	{
		for (size_t j = 0; j < i; j++)
		{
			double *pher = pheromones->getPtr(i, j);
			*pher = *pher * (p);
			if (*pher < MIN_PHER)
			{
				*pher = MIN_PHER;
			}
		}
	}
	// Add ant pheromones

	for (size_t a = 1; a < best_ant.cutted.size(); a++)
	{
		double *pher = pheromones->getPtr(best_ant.cutted[a - 1], best_ant.cutted[a]);
		*pher = *pher + PHER_INCREASE; 
		if (*pher > MAX_PHER)
		{
			*pher = MAX_PHER;
		}
	}
}

bool choose_tree(int ant_num)
{
	vector<double> p;
	p.resize(treeCount + 1);
	double total = 0.0;
	for (int j = 1; j <= treeCount; j++)
	{
		if (ants[ant_num]->i != j && !cutted[j] && ants[ant_num]->t >= (calculate_distance(ants[ant_num]->i, j) + trees[j].d))
		{
			p[j] = pow(pheromones->get(ants[ant_num]->i, j), alfa) * pow((1.0 / (distances->get(ants[ant_num]->i, j) + trees[j].d)), beta) * pow(*max_element(cut_value[j], cut_value[j] + 4), gam);
			total += p[j];
		}
		else
		{
			p[j] = 0;
		}
	}
	if (total == 0.0)
		return false;
	double r = rand() % 100 / 100.0;
	r *= total;
	double sum = 0.0;
	for (int j = 1; j <= treeCount; j++)
	{
		sum += p[j];
		if (r < sum)
		{
			ants[ant_num]->cutted.push_back(j);
			int direction = distance(cut_value[j], max_element(cut_value[j], cut_value[j] + 4));
			ants[ant_num]->directions.push_back(direction);
			int cost = distances->get(ants[ant_num]->i, j) + trees[j].d;
			ants[ant_num]->t -= cost;
			ants[ant_num]->res += cutTree(j, direction);
			ants[ant_num]->i = j;
			ants[ant_num]->total += cost;
			//cutted[j] = true;
			break;
		}
	}
	return true;
}

void print_result(){
	for (int i = 1; i < best_ant.cutted.size(); i++)
	{
		int horizontal = trees[best_ant.cutted[i]].x - trees[best_ant.cutted[i - 1]].x;
		int vertical = trees[best_ant.cutted[i]].y - trees[best_ant.cutted[i - 1]].y;
		while (horizontal > 0)
		{
			cout << "move right\n";
			horizontal--;
		}
		while (horizontal < 0)
		{
			cout << "move left\n";
			horizontal++;
		}
		while (vertical > 0)
		{
			cout << "move up\n";
			vertical--;
		}
		while (vertical < 0)
		{
			cout << "move down\n";
			vertical++;
		}
		cout << "cut " << directions[best_ant.directions[i]] << "\n";
	}
}

void loadFromStream(istream &stream) {
	stream >> timeLimit >> mapSize >> treeCount;
	trees.resize(treeCount + 1);
	for (int i = 1; i <= treeCount; i++)
	{
		int x, y, h, d, c, p;
		stream >> x >> y >> h >> d >> c >> p;
		forest[x][y] = i; // assign tree index from trees vector to map field
		trees[i].setValues( x, y, h, d, c, p );
	}
}

void runGreedy()
{
	best_ant.i = 0;
	best_ant.t = timeLimit;
	best_ant.res = 0;
	best_ant.total = 0;
	best_ant.cutted.push_back(0);
	best_ant.directions.push_back(-1);
	/*    for (int a = 1; a <= treeCount; a++)
	{
	cutted[a] = false;
	}
	cutted[0] = true;*/
	while (best_ant.t > 0)
	{
		calculate_cut_values(greedyCutValues);
		double best_value = -1;
		int best_tree = -1;
		int cut_direction = -1;
		for (int j = 1; j <= treeCount; j++)
		{
			int cost = trees[j].d + calculate_distance(best_ant.i, j);
			int direction = distance(greedyCutValues[j], max_element(greedyCutValues[j], greedyCutValues[j] + 4));
			if (!cutted[j] && best_ant.t >= cost)
			{
				double res = greedyCutValues[j][direction] / (double)cost;
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
		else{
			best_ant.cutted.push_back(best_tree);
			best_ant.directions.push_back(cut_direction);
			int cost = trees[best_tree].d + calculate_distance(best_ant.i, best_tree);
			best_ant.t -= cost;
			best_ant.res += cutTree(best_tree, cut_direction);
			best_ant.i = best_tree;
			best_ant.total += cost;
		}
	}
	best = best_ant.res;
}


int main(int argc, char** argv)
{
	const clock_t begin_time = clock();
	loadFromStream(cin);

	
	srand(static_cast<unsigned int>(time(0)));
	trees[0].setValues( 0, 0, 0, 0, 0, 0 ); 
	calculate_cut_values(cut_value);
	runGreedy();
	distances = new Distances(treeCount + 1);
	pheromones = new Pheromones(treeCount + 1, best_ant);

	int iters = 0;
	int stagnantIters = STAGNANT_ITERATIONS;
	while (true)
	{
		iters++;
		int scoreSum = 0;
		ant *iterBestAnt = 0;
		int iterBestScore = 0;
		for (int i = 0; i < ANTS; i++)
		{	
			ants[i] = new ant();
			ants[i]->i = 0;
			ants[i]->t = timeLimit;
			ants[i]->res = 0;
			ants[i]->total = 0;
			ants[i]->cutted.push_back(0);
			ants[i]->directions.push_back(-1);
			for (int a = 0; a <= treeCount; a++)
			{
				cutted[a] = false;
			}
			cutted[0] = true;
			while (ants[i]->t > 0 && choose_tree(i));

			if (ants[i]->res > best)
			{
				best = ants[i]->res;
				best_ant = *ants[i];
				stagnantIters = STAGNANT_ITERATIONS;
			}

			if (ants[i]->res > iterBestScore)
			{
				iterBestAnt = ants[i];
			}
			scoreSum += ants[i]->res;	
			if ((treeCount <= 100 && float(clock() - begin_time) / CLOCKS_PER_SEC > TIME1) ||
				(treeCount <= 1000 && float(clock() - begin_time) / CLOCKS_PER_SEC > TIME2) ||
				(treeCount <= 10000 && float(clock() - begin_time) / CLOCKS_PER_SEC > TIME3))
			{
				print_result();
				return 0;
			}

		}
		if (!stagnantIters--)
		{
			pheromones->reset();
			stagnantIters = STAGNANT_ITERATIONS;
		}
		updatePheromones(iterBestAnt);
	}
}
