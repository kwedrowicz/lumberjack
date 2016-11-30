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


const double TIME_LIMIT = 555555.95;

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
    vector<int> cutted;
    vector<int> directions;
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
	double **matrix;
	int size;
public:
	Pheromones(int treeCount) {
		size = treeCount;
		matrix = new double*[treeCount-1];
		for (int i = 0; i < treeCount-1; i++) {
			matrix[i] = new double[treeCount-i-1];
		}
		for (int i = 0; i < treeCount - 1; i++)
		{
			for (int j = 0; j < treeCount - i - 1; j++)
			{
				matrix[i][j] = 1;
			}
		}
	}
	~Pheromones() {
		for (int i = 0; i < size-1; i++) {
			delete[] matrix[i];
		}
		delete[] matrix;
	}
	double get(int i, int j) {
		if (i < j) {
			return matrix[i][size-j-1];
		}
		else {
			return matrix[j][size-i-1];
		}
	}
	void set(int i, int j, double val) {
		if (i < j) {
			matrix[i][size-j-1] = val;
		}
		else {
			matrix[j][size-i-1] = val;
		}
	}
};

string directions[] = {"down", "right", "up", "left"};

int timeLimit, mapSize, treeCount;

double alfa = 0.5;
double beta = 0.5;
double gam = 0.5;
double p = 0.5;
int iterations = ITERATIONS;
int best = 0;

vector<tree> trees;
Distances *distances;
Pheromones *pheromones;
//double matrix[MAXK+1][MAXK+1];
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
          //  cout << sum <<" ";
        }
       // cout <<endl;
    }
}

void updatePheromones() {
	for (size_t i = 0; i < trees.size(); i++)
	{
		for (size_t j = 0; j < i; j++)
		{
			pheromones->set(i, j, (1 - p)* pheromones->get(i, j)); // evaporation
		}
	}
	// Add ant pheromones
	for (int ant = 0; ant < ANTS; ant++)
	{
		for (size_t a = 1; a < ants[ant]->cutted.size(); a++)
		{
			double newval = pheromones->get(ants[ant]->cutted[a - 1], ants[ant]->cutted[a]) + (1 / double(ants[ant]->total)) + (ants[ant]->res / (double)best);
		//	cout << newval << "  ";
			pheromones->set(ants[ant]->cutted[a - 1], ants[ant]->cutted[a], newval);
		} 
	}
/*	for (size_t i = 0; i < trees.size(); i++)
	{
		for (size_t j = 0; j < i; j++)
		{
			if (pheromones->get(i, j) > 0.5)
			cout << pheromones->get(i, j) << endl; // evaporation
		}
	} */
}

bool choose_tree(int ant_num)
{
    //cout <<"ANT "<< ant_num <<" IN TREE: "<<ants[ant_num]->i<<endl;
    vector<double> p;
    p.resize(treeCount+1);
    double total = 0.0;
	double down = 0;
	for (int l = 1; l <= treeCount; l++)
	{
		if (!cutted[l]){
			down += pow(pheromones->get(l, ants[ant_num]->i), alfa) * pow((1.0 / (distances->get(ants[ant_num]->i, l) + trees[l].d)), beta) * pow(*max_element(cut_value[l], cut_value[l] + 4), gam);
		}
	}
    for(int j = 1; j <= treeCount; j++)
    {

        //cout << cutted[j] << " "<<ants[ant_num]->t<<" "<<calculate_distance(ants[ant_num]->i,j) + trees[j].d<< endl;
        if(ants[ant_num]->i != j && !cutted[j] && ants[ant_num]->t >= (calculate_distance(ants[ant_num]->i,j) + trees[j].d) )
        {
			double up = pow(pheromones->get(ants[ant_num]->i, j), alfa) * pow((1.0 / (distances->get(ants[ant_num]->i, j) + trees[j].d)), beta) * pow(*max_element(cut_value[j], cut_value[j] + 4), gam);
            p[j] = up / down;
            total += p[j];
        }
        else
        {
            p[j] = 0;
        }
        //cout <<"P["<<j<<"] = "<<p[j]<<endl;
    }
    if(total == 0.0)
        return false;
    double r = rand() % 100 / 100.0;
    r *= total;
    double sum = 0.0;
    //cout << r<< " "<<total<<"\n";
    for(int j = 1; j <= treeCount; j++)
    {
        sum += p[j];
        if(r < sum)
        {
            ants[ant_num]->cutted.push_back(j);
            int direction = distance(cut_value[j], max_element(cut_value[j], cut_value[j]+4));
            ants[ant_num]->directions.push_back(direction);
            int cost = distances->get(ants[ant_num]->i, j) + trees[j].d;
            ants[ant_num]->t -= cost;
            ants[ant_num]->res += cutTree(j,direction);
            ants[ant_num]->i = j;
            ants[ant_num]->total += cost;
            //cutted[j] = true;
            break;
        }
    }
    return true;
}

void print_best(clock_t begin_time)
{
    cout <<"BEST SCORE: "<<best << " TIME: "<< best_ant.t <<endl << "PATH:\n";
    for(size_t i = 0; i < best_ant.cutted.size(); i++)
    {
        cout << best_ant.cutted[i];
		if (best_ant.directions[i] >= 0 && best_ant.directions[i] <= 3) {
			cout << "(" << directions[best_ant.directions[i]] << ")";
		}
		if (i < best_ant.cutted.size() - 1) {
			cout << " -> ";
		}
    }
    cout <<"\n";
    cout << float( clock () - begin_time ) /  CLOCKS_PER_SEC <<"\n";
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
	/*if (argc == 2) {
		// load from file
		ifstream file;
		file.open(argv[1]);
		if (!file.is_open())
		  return 100;
		loadFromStream(file);
		file.close();
	}
	else {*/
		// load from console
	//	cout << "Input forest description:" << endl;
		loadFromStream(cin);
	//}
	//clock_t loadTime = float(clock() - begin_time);
	//cout << "FILE LOAD TOOK: " << loadTime / CLOCKS_PER_SEC << "\n";
   /* cout <<"FOREST\n";
    for(int i = 0; i < mapSize; i++)
    {
        for(int j = 0; j < mapSize; j++)
        {
            cout << forest[i][j] <<" ";
        }
        cout <<"\n";
    } */
    srand(static_cast<unsigned int>(time(0)));
    trees[0] = {0,0,0,0,0,0}; // create fake tree on starting point
	distances = new Distances(treeCount + 1);
	pheromones = new Pheromones(treeCount + 1);
    calculate_cut_values();
	//cout << "CALCULATING TREE VALUES TOOK: " << float(clock() - loadTime) / CLOCKS_PER_SEC << "\n";
	//cout << "CUT VALUES DOWN" << endl;
	//for (int i = 0; i < treeCount; i++) {
	//	cout << i << ": " << cut_value[i][0] << endl;
	//}
    while(true)
    {
        //cout << "ITERATION "<<ITERATIONS-iterations<<"\n";
		//const clock_t iterBeginTime = clock();
      /*  for(size_t i = 0; i < trees.size(); i++)
        {
            for(size_t j = 0; j < trees.size(); j++)
            {
				if (i == j) {
					cout << 0 << " ";
				}
				else if (i < j) {
					cout << distances->get(i, j) << " ";
				}
				else {
					cout << pheromones->get(i, j) << " ";
				}
            }
            cout <<"\n";
        } */
        for(int i = 0; i < ANTS; i++)
        {
			//const clock_t antBeginTime = clock();
            //cout << "ANT NUMBER "<<i<<endl;
            ants[i] = new ant();
            ants[i]->i = 0;
            ants[i]->t = timeLimit;
            ants[i]->res = 0;
            ants[i]->total = 0;
            ants[i]->cutted.push_back(0);
            ants[i]->directions.push_back(-1);
            for(int a = 0; a <= treeCount; a++)
            {
                cutted[a] = false;
            }
            cutted[0] = true;
			while (ants[i]->t > 0 && choose_tree(i));         
            if(ants[i]->res > best)
            {
                //cout <<"NEW BEST "<<best<<" -> "<<ants[i]->res<<endl;
                best = ants[i]->res;
                best_ant = *ants[i];
            }
            /*if(float( clock () - begin_time ) /  CLOCKS_PER_SEC > TIME_LIMIT)
            {
                print_result();
                return 0;
            }*/

            if(
                    (treeCount <= 100 && float( clock () - begin_time ) /  CLOCKS_PER_SEC > TIME1) ||
                    (treeCount <= 1000 && float( clock () - begin_time ) /  CLOCKS_PER_SEC > TIME2) ||
                    (treeCount <= 10000 && float( clock () - begin_time ) /  CLOCKS_PER_SEC > TIME3)
            ){
                print_result();
                return 0;
            }

			//cout << "ANT NUMBER "<<i<<" TOOK: " << float(clock() - antBeginTime) / CLOCKS_PER_SEC << "\n";
        }
		//const clock_t evapBeginTime = clock();
		updatePheromones();
		//cout << "EVAPORATION " << ITERATIONS - iterations << " TOOK: " << float(clock() - evapBeginTime) / CLOCKS_PER_SEC << "\n";
		//cout << "ITERATION " << ITERATIONS - iterations << " TOOK: " << float(clock() - iterBeginTime) / CLOCKS_PER_SEC << "\n";
    }
    //print_best(begin_time);
    print_result();
    return 0;
}
