#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>
#include <algorithm>

#define MAXK 10000
#define MAXN 1000
#define ANTS 25
#define ITERATIONS 20
#define TIME1 0.95
#define TIME2 9.0
#define TIME3 58.0

using namespace std;

struct tree{
	// x,y - position on map
	// h - height
	// d - diameter
	// c - weight per volume unit
	// p - price/value per volume unit
	// volume = h * d
    int x, y, h, d, c, p;
};

struct ant{
	// i - index of current tree the ant is in
	// t - time left 
	// res - result
	// total - distance travelled
    int i, t, res, total;
    int x,y;
    vector<int> cutted;
    vector<int> directions;
    vector<pair<int,int> > moves;
};

int calculate_distance(int,int);

class Distances {
	int **matrix;
	int size;
public:
	Distances(int treeCount) {
		size = treeCount;
		matrix = new int*[treeCount-1];
		for (int i = 0; i < treeCount-1; i++) {
			matrix[i] = new int[treeCount-i-1];
		}
		for (int i = 0; i < treeCount - 1; i++)
		{
			for (int j = 0; j < treeCount - i - 1; j++)
			{
				matrix[i][j] = calculate_distance(i, treeCount-j-1);
			}
		}
	}
	~Distances() {
		for (int i = 0; i < size-1; i++) {
			delete[] matrix[i];
		}
		delete[] matrix;
	}
	int get(int i, int j) {
		if (i < j) {
			return matrix[i][size-j-1];
		}
		else {
			return matrix[j][size-i-1];
		}
	}
};

class Pheromones {
	vector< vector< vector<double> > > map;
    unsigned int size;
public:
	Pheromones(unsigned int mapSize, vector<tree> trees) {
		size = mapSize+1;
		map.resize(size);
		for (int i = 0; i < size; i++) {

			map[i].resize(size);
            for(int j = 0; j < size; j++)
            {
                map[i][j].resize(2);
            }
		}
		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < size; j++)
			{
				for(int k = 0; k < 2; k++)
                    map[i][j][k] = 1;
			}
		}
        int max_value = 1;
        for(int i = 1; i < trees.size(); i++){
            int tree_value = trees[i].h * trees[i].d * trees[i].p;
            max_value = max(tree_value, max_value);
        }
        for(int i = 1; i < trees.size(); i++){
            int sx = max(trees[i].x - trees[i].h,0);
            int sy = max(trees[i].y - trees[i].h,0);
            int ex = min(trees[i].x + trees[i].h, int(size-1));
            int ey = min(trees[i].y + trees[i].h, int(size-1));
            int tree_value = trees[i].h * trees[i].d * trees[i].p;
            for(int j = sy; j <= ey; j++)
            {
                for(int k = sx; k <= ex; k++)
                {
                    int val = tree_value/max_value * max(trees[i].h-(abs(trees[i].y-j)+abs(trees[i].x-k)),0);
                    for(int m = 0; m < 2; m++)
                    {
                        map[k][j][m] += val;
                    }
                }
            }
        }

	}
	~Pheromones() {
		map.clear();
	}
	double get(int i, int j, int k) {
        if(k < 2)
		    return map[i][j][k];
        else if(k == 2)
            return map[i][j-1][0];
        else
            return map[i-1][j][1];
	}
    double get(pair<int,int> p1, pair<int,int> p2){
        if(p1.first == p2.first)
            return map[p1.first][min(p1.second, p2.second)][0];
        else
            return map[min(p1.first, p2.first)][p1.second][1];
    }
	void set(int i, int j, int k, double val) {
        if(k < 2)
            map[i][j][k] = val;
        else if(k == 2)
            map[i][j-1][0] = val;
        else
            map[i-1][j][1] = val;
	}
    void set(pair<int,int> p1, pair<int,int> p2, double val){
        if(p1.first == p2.first)
            map[p1.first][min(p1.second, p2.second)][0] = val;
        else
            map[min(p1.first, p2.first)][p1.second][1] = val;
    }
    bool isValidPostion(int i, int j, int k){
        if(k == 2)
            j--;
        if(k == 3)
            i--;
        if(i < 0 || j < 0 || i >= size || j >= size)
            return false;
        return true;
    }
};

string directions[] = {"down", "right", "up", "left"};

int timeLimit, mapSize, treeCount;

double alfa = 0.5;
double beta = 0.5;
double gam = 0.5;
double p = 0.5;
int best = 0;

vector<tree> trees;
Distances *distances;
Pheromones *pheromones;
int forest[MAXN][MAXN];
bool cutted[MAXK+1];
int cut_value[MAXK+1][4];
ant* ants[ANTS];
ant best_ant;

int calculate_distance(int i, int j)
{
    return abs(trees[i].x-trees[j].x) + abs(trees[i].y-trees[j].y);
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
    int sum = tree_value(i);
    int height = trees[i].h;
    int x = trees[i].x;
    int y = trees[i].y;
    int cur_tree = i;
	cutted[cur_tree] = true;
    int dx, dy;
    if(direction == 0)
    {
        dx = 0;
        dy = -1;
    }
    if(direction == 1)
    {
        dx = 1;
        dy = 0;
    }
    if(direction == 2)
    {
        dx = 0;
        dy = 1;
    }
    if(direction == 3)
    {
        dx = -1;
        dy = 0;
    }
    height--;
    while(height > 0)
    {
        x += dx;
        y += dy;
        if(x < 0 || y < 0 || x >= mapSize || y >= mapSize)
            break;
        if(forest[x][y] > 0)
        {
            if(!cutted[forest[x][y]] && tree_weight(cur_tree) > tree_weight(forest[x][y]))
            {
                sum += tree_value(forest[x][y]);
                height = trees[forest[x][y]].h;
                cur_tree = forest[x][y];
				cutted[cur_tree] = true;
            }
            else
                break;
        }
        height--;
    }
    return sum;
}
void calculate_cut_values()
{
    for(int i = 1; i <= treeCount; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            int sum = tree_value(i);
            int height = trees[i].h;
            int x = trees[i].x;
            int y = trees[i].y;
            int cur_tree = i;
			int dx = 0;
			int	dy = 0;
            if(j == 0)
            {
                dy = -1;
            }
            if(j == 1)
            {
                dx = 1;
            }
            if(j == 2)
            {
                dy = 1;
            }
            if(j == 3)
            {
                dx = -1;
            }
            height--;
            while(height > 0)
            {
                x += dx;
                y += dy;
                if(x < 0 || y < 0 || x >= mapSize || y >= mapSize)
                    break;
                if(forest[x][y] > 0)
                {
                    if(tree_weight(cur_tree) > tree_weight(forest[x][y]))
                    {
                        sum += tree_value(forest[x][y]);
                        height = trees[forest[x][y]].h;
                        cur_tree = forest[x][y];
                        cutted[forest[x][y]] = true;
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

void updatePheromones() {
    //cout << "UPDATE 1"<<endl;
	for (int i = 0; i < mapSize+1; i++)
	{
		for (int j = 0; j < mapSize+1; j++)
		{
            for(int k = 0; k < 2; k++)
			    pheromones->set(i, j, k, (1 - p)* pheromones->get(i, j, k)); // evaporation
		}
	}
    //cout << "UPDATE 2"<<endl;
	// Add ant pheromones
	for (int ant = 0; ant < ANTS; ant++)
	{
		for (size_t a = 1; a < ants[ant]->moves.size(); a++)
		{
			double newval = pheromones->get(ants[ant]->moves[a - 1], ants[ant]->moves[a]) + (1 / double(ants[ant]->total)) + (ants[ant]->res / (double)best);
			pheromones->set(ants[ant]->moves[a - 1], ants[ant]->moves[a], newval);
		} 
	}
    //cout << "UPDATE END"<<endl;
}

pair<int,int> getPosition(int x, int y, int d){
    switch(d){
        case 0:
            y++;
            break;
        case 1:
            x++;
            break;
        case 2:
            y--;
            break;
        case 3:
            x--;
            break;
        default:
            break;
    }
    return make_pair(x,y);
};

bool choose_tree(int ant_num)
{
    //nie działa ta funkcja :(

    //cout <<"ANT "<< ant_num <<" IN POS: "<<ants[ant_num]->x <<" "<<ants[ant_num]->y<<endl;
    vector<double> p;
    p.resize(4);
    double total = 0.0;
	double down = 0;
	for (int l = 0; l < 4; l++)
	{
        pair<int,int> cur_pos = getPosition(ants[ant_num]->x, ants[ant_num]->y, l);
        int tree_num = forest[cur_pos.first][cur_pos.second];
        if(pheromones->isValidPostion(ants[ant_num]->x, ants[ant_num]->y, l)){
            //cout << "VALID"<<endl;
            if(tree_num > 0 && !cutted[tree_num])
                down += pow(pheromones->get(ants[ant_num]->x, ants[ant_num]->y, l), alfa) * pow((1.0 / (1 + trees[tree_num].d)), beta) * pow(*max_element(cut_value[tree_num], cut_value[tree_num] + 4), gam);
            else
                down += pow(pheromones->get(ants[ant_num]->x, ants[ant_num]->y, l), alfa);
        }
	}
    //cout << "DOWN = "<<down<<endl;
    for(int j = 0; j < 4; j++)
    {
        double up = 0;
        if(pheromones->isValidPostion(ants[ant_num]->x, ants[ant_num]->y, j)) {
            pair<int, int> cur_pos = getPosition(ants[ant_num]->x, ants[ant_num]->y, j);
            int tree_num = forest[cur_pos.first][cur_pos.second];

            if (cur_pos == ants[ant_num]->moves.back())
                up = 0;
            else if (tree_num > 0 && !cutted[tree_num]) {
                if (ants[ant_num]->t >= (1 + trees[j].d))
                    up = pow(pheromones->get(ants[ant_num]->x, ants[ant_num]->y, j), alfa) *
                         pow((1.0 / (1 + trees[j].d)), beta) *
                         pow(*max_element(cut_value[tree_num], cut_value[tree_num] + 4), gam);
                else
                    up = 0;
            } else
                up = pow(pheromones->get(ants[ant_num]->x, ants[ant_num]->y, j), alfa);
        }

        p[j] = up/down;
        //cout <<"P["<<j<<"] = "<<p[j]<<endl;
        total += p[j];
    }
    if(total == 0.0)
        return false;
    double r = rand() % 100 / 100.0;
    r *= total;
    double sum = 0.0;
    //cout << r<< " "<<total<<"\n";
    for(int j = 0; j < 4; j++)
    {
        sum += p[j];
        if(r < sum)
        {

            pair<int,int> cur_pos = getPosition(ants[ant_num]->x, ants[ant_num]->y, j);
            int tree_num = forest[cur_pos.first][cur_pos.second];

            ants[ant_num]->moves.push_back(cur_pos);
            ants[ant_num]->x = cur_pos.first;
            ants[ant_num]->y = cur_pos.second;
            int cost;

            if(tree_num > 0 && !cutted[tree_num]){
                ants[ant_num]->cutted.push_back(tree_num);
                int direction = distance(cut_value[tree_num], max_element(cut_value[tree_num], cut_value[tree_num]+4));
                ants[ant_num]->directions.push_back(direction);
                cost = 1 + trees[tree_num].d;
                ants[ant_num]->res += cutTree(tree_num,direction);
            } else{
                cost = 1;
            }
            ants[ant_num]->t -= cost;
            ants[ant_num]->total += cost;
            break;
        }
    }
    return true;
}


void loadFromStream(istream &stream) {
	stream >> timeLimit >> mapSize >> treeCount;
	trees.resize(treeCount + 1);
	for (int i = 1; i <= treeCount; i++)
	{
		int x, y, h, d, c, p;
		stream >> x >> y >> h >> d >> c >> p;
		forest[x][y] = i; // assign tree index from trees vector to map field
		trees[i] = { x, y, h, d, c, p };
	}
}

void print_result(){
    for(int i = 1; i < best_ant.cutted.size(); i++)
    {
        int horizontal = trees[best_ant.cutted[i]].x-trees[best_ant.cutted[i-1]].x;
        int vertical = trees[best_ant.cutted[i]].y-trees[best_ant.cutted[i-1]].y;
        while(horizontal > 0)
        {
            cout << "move right\n";
            horizontal--;
        }
        while(horizontal < 0)
        {
            cout << "move left\n";
            horizontal++;
        }
        while(vertical > 0)
        {
            cout << "move up\n";
            vertical--;
        }
        while(vertical < 0)
        {
            cout << "move down\n";
            vertical++;
        }
        cout << "cut "<<directions[best_ant.directions[i]]<<"\n";
    }
}

int main(int argc, char** argv)
{
    const clock_t begin_time = clock();
	loadFromStream(cin);
    srand(static_cast<unsigned int>(time(0)));
    trees[0] = {0,0,0,0,0,0}; // create fake tree on starting point
	distances = new Distances(treeCount + 1);
	pheromones = new Pheromones(mapSize, trees);
    calculate_cut_values();
    while(true)
    {
        for(int i = 0; i < ANTS; i++)
        {
            ants[i] = new ant();
            ants[i]->i = 0;
            ants[i]->x = 0;
            ants[i]->y = 0;
            ants[i]->t = timeLimit;
            ants[i]->res = 0;
            ants[i]->total = 0;
            ants[i]->cutted.push_back(0);
            ants[i]->directions.push_back(-1);
            ants[i]->moves.push_back(make_pair(0,0));
            for(int a = 0; a <= treeCount; a++)
            {
                cutted[a] = false;
            }
            cutted[0] = true;
			while (ants[i]->t > 0 && choose_tree(i));         
            if(ants[i]->res > best)
            {
                best = ants[i]->res;
                best_ant = *ants[i];
            }
            if(
                    (treeCount <= 100 && float( clock () - begin_time ) /  CLOCKS_PER_SEC > TIME1) ||
                    (treeCount <= 1000 && float( clock () - begin_time ) /  CLOCKS_PER_SEC > TIME2) ||
                    (treeCount <= 10000 && float( clock () - begin_time ) /  CLOCKS_PER_SEC > TIME3)
            ){
                print_result();
                return 0;
            }
        }
		updatePheromones();
    }
}
