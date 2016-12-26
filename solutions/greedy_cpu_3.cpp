#include <iostream>
#include <vector>
#include <ctime>
#include <algorithm>

#define MAXK 10000
#define MAXN 1000

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


string directions[] = {"down", "right", "up", "left"};

int timeLimit, mapSize, treeCount;
int best = 0;

vector<tree> trees;

int forest[MAXN][MAXN];
bool cutted[MAXK+1];
int cut_value[MAXK+1][4];
ant best_ant;

vector<int> last_cutted;
int last_earned;
int last_time;

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
    last_cutted.clear();
    int sum = tree_value(i);
    int height = trees[i].h;
    int x = trees[i].x;
    int y = trees[i].y;
    int cur_tree = i;
    cutted[cur_tree] = true;
    last_cutted.push_back(cur_tree);
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
        if(forest[x][y] > 0 && !cutted[forest[x][y]])
        {
            if(tree_weight(cur_tree) > tree_weight(forest[x][y]))
            {
                sum += tree_value(forest[x][y]);
                height = trees[forest[x][y]].h;
                cur_tree = forest[x][y];
                cutted[cur_tree] = true;
                last_cutted.push_back(cur_tree);
            }
            else
                break;
        }
        height--;
    }
    last_earned = sum;
    return sum;
}
void calculate_cut_values()
{
    for(int i = 1; i <= treeCount; i++)
    {
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
                    } else
                        break;
                }
                height--;
            }
            cut_value[i][j] = sum;
            //  cout << sum <<" ";
        }
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
        trees[i] = { x, y, h, d, c, p };
    }
}

void print_result(){
    //cout << "Czas do końca: "<<best_ant.t<<"\n";
    /*for(int i = 1; i < best_ant.cutted.size(); i++){
        cout << best_ant.cutted[i] <<" ";
    }*/
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
    trees[0] = {0,0,0,0,0,0}; // create fake tree on starting point
    best_ant.i = 0;
    best_ant.t = timeLimit;
    best_ant.res = 0;
    best_ant.total = 0;
    best_ant.cutted.push_back(0);
    best_ant.directions.push_back(-1);
    for(int a = 0; a <= treeCount; a++)
    {
        cutted[a] = false;
    }
    cutted[0] = true;
    while(best_ant.t > 0)
    {
        calculate_cut_values();
        double best_value = -1;
        int best_tree = -1;
        int cut_direction = -1;
        for(int j = 1; j <= treeCount; j++)
        {
            int cost = trees[j].d+calculate_distance(best_ant.i, j);
            int direction = distance(cut_value[j], max_element(cut_value[j], cut_value[j]+4));
            if(!cutted[j] && best_ant.t >= cost)
            {
                double res = cut_value[j][direction]/(double)cost;
                if(res > best_value)
                {
                    best_value = res;
                    best_tree = j;
                    cut_direction = direction;
                }
            }
        }
        if(best_value < 0)
            break;
        else{
            best_ant.cutted.push_back(best_tree);
            best_ant.directions.push_back(cut_direction);
            int cost = trees[best_tree].d+calculate_distance(best_ant.i, best_tree);
            best_ant.t -= cost;
            last_time = cost;
            best_ant.res += cutTree(best_tree,cut_direction);
            best_ant.i = best_tree;
            best_ant.total += cost;
        }
    }

    best_ant.cutted.pop_back();
    best_ant.directions.pop_back();
    best_ant.t += last_time;
    best_ant.res -= last_earned;
    best_ant.i = best_ant.cutted.back();
    for(int i = 0; i < last_cutted.size(); i++){
        cutted[last_cutted[i]] = false;
    }
    calculate_cut_values();
    double best_value = -1;
    int best_tree = -1;
    int cut_direction = -1;
    for(int j = 1; j <= treeCount; j++)
    {
        int cost = trees[j].d+calculate_distance(best_ant.i, j);
        int direction = distance(cut_value[j], max_element(cut_value[j], cut_value[j]+4));
        if(!cutted[j] && best_ant.t >= cost)
        {
            double res = cut_value[j][direction];
            if(res > best_value)
            {
                best_value = res;
                best_tree = j;
                cut_direction = direction;
            }
        }
    }
    best_ant.cutted.push_back(best_tree);
    best_ant.directions.push_back(cut_direction);
    int cost = trees[best_tree].d+calculate_distance(best_ant.i, best_tree);
    best_ant.t -= cost;
    best_ant.res += cutTree(best_tree,cut_direction);
    best_ant.i = best_tree;
    best_ant.total += cost;

    print_result();
    return 0;
}
