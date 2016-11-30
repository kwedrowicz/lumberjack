#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>
#include <algorithm>

#define MAXK 10000
#define MAXN 1000
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
ant best_ant, current_ant, last_ant;

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

void generateSolution(){
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
            best_ant.res += cutTree(best_tree,cut_direction);
            best_ant.i = best_tree;
            best_ant.total += cost;
        }
    }
}

void recalculateSolution(){
    for(int i = 1; i <= treeCount; i++)
        cutted[i] = false;
    current_ant.res = 0;
    current_ant.t = timeLimit;
    for(int i = 1; i < current_ant.cutted.size(); i++)
    {
        if(cutted[current_ant.cutted[i]]){
            current_ant.cutted.erase(current_ant.cutted.begin()+i);
            current_ant.directions.erase(current_ant.directions.begin()+i);
            i--;
            continue;
        }
        current_ant.t -= (calculate_distance(current_ant.cutted[i], current_ant.cutted[i-1])+trees[current_ant.cutted[i]].d);
        current_ant.res += cutTree(current_ant.cutted[i], current_ant.directions[i]);
    }
}

void localChange(){
    unsigned long size = current_ant.cutted.size();
    long r = rand() % (size-1) + 1;
    int additional_time = calculate_distance(current_ant.cutted[r-1],current_ant.cutted[r]);
    if(r < size-1)
        additional_time += calculate_distance(current_ant.cutted[r],current_ant.cutted[r+1]);
    additional_time += trees[current_ant.cutted[r]].d;
    current_ant.t += additional_time;
    cutted[current_ant.cutted[r]] = false;
    vector<int> possible_choose;
    int consume_time;
    for(int i = 1; i <= treeCount; i++){
        if(!cutted[i]){
            consume_time = trees[i].d;
            consume_time += calculate_distance(i, current_ant.cutted[r-1]);
            if(r < size-1)
                consume_time += calculate_distance(i, current_ant.cutted[r+1]);
            if(consume_time <= current_ant.t){
                possible_choose.push_back(i);
            }
        }
    }
    int chooose_rand = int(rand() % possible_choose.size());
    current_ant.cutted[r] = possible_choose[chooose_rand];
    int direction = (int)distance(cut_value[possible_choose[chooose_rand]], max_element(cut_value[possible_choose[chooose_rand]], cut_value[possible_choose[chooose_rand]]+4));
    current_ant.directions[r] = direction;
    recalculateSolution();
    while(current_ant.t > 0){
        possible_choose.clear();
        size = current_ant.cutted.size();
        for(int i = 1; i <= treeCount; i++){
            if(!cutted[i]){
                consume_time = trees[i].d;
                consume_time += calculate_distance(i, current_ant.cutted[r]);
                if(r < size-1)
                    consume_time += calculate_distance(i, current_ant.cutted[r+1]);
                if(consume_time <= current_ant.t){
                    possible_choose.push_back(i);
                }
            }
        }
        if(possible_choose.size() > 0){
            chooose_rand = int(rand() % possible_choose.size());
            current_ant.cutted.insert(current_ant.cutted.begin()+r+1, possible_choose[chooose_rand]);
            direction = (int)distance(cut_value[possible_choose[chooose_rand]], max_element(cut_value[possible_choose[chooose_rand]], cut_value[possible_choose[chooose_rand]]+4));
            current_ant.directions.insert(current_ant.directions.begin()+r+1, direction);
            r++;
        }
        else
            break;
        recalculateSolution();
    }
}

int main(int argc, char** argv)
{
    const clock_t begin_time = clock();
    loadFromStream(cin);
    srand(static_cast<unsigned int>(time(0)));
    trees[0] = {0,0,0,0,0,0}; // create fake tree on starting point
    calculate_cut_values();
    srand( time( NULL ) );
    generateSolution();
    current_ant = best_ant;
    while(true)
    {
        localChange();
        //cout << best_ant.res<<" "<<current_ant.res <<"\n";
        if(current_ant.res > last_ant.res)
            last_ant = current_ant;
        else{
            double bound = 0.7;
            double past_time = float( clock () - begin_time ) /  CLOCKS_PER_SEC;
            double full_time;
            double divider;
            if(treeCount <= 100)
                full_time = TIME1;
            else if(treeCount <= 1000)
                full_time = TIME2;
            else
                full_time = TIME3;
            double part = past_time/full_time;
            if(part < bound){
                // y = 1,4286x + 1
                divider = 1.4286*part+1;
            }
            else{
                // y = 26,667x - 16,667
                divider = 26.667*part-16.667;
            }
            double prob = pow(M_E, (current_ant.res/(double)last_ant.res)/divider) - 1;
            if(prob*100 > (rand() % 100)){
                last_ant = current_ant;
            }
        }
        if(current_ant.res > best_ant.res)
            best_ant = current_ant;
        if(
                (treeCount <= 100 && float( clock () - begin_time ) /  CLOCKS_PER_SEC > TIME1) ||
                (treeCount <= 1000 && float( clock () - begin_time ) /  CLOCKS_PER_SEC > TIME2) ||
                (treeCount <= 10000 && float( clock () - begin_time ) /  CLOCKS_PER_SEC > TIME3)
                ){
            print_result();
            return 0;
        }
    }
}

