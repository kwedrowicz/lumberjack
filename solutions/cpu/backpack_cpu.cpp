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


int main(int argc, char** argv)
{
    const clock_t begin_time = clock();
    loadFromStream(cin);
    trees[0] = {0,0,0,0,0,0}; // create fake tree on starting point
    calculate_cut_values();
    ant tab[10005][5005];
    for(int j = 0; j <= timeLimit; j++){
        tab[0][j].cutted.push_back(i);
        tab[0][j].directions.push_back(-1);
    }
    for(int i = 1; i <= treeCount; i++)
    {
        for(int j = 0; j <= timeLimit; j++)
        {
            int best_value = -1;
            int best_position = -1;
            for(int k = 1; k < j; j++)
            {
                if(trees[i].d+calculate_distance(i,tab[i-1][k].cutted.back())+k <= j)
                {
                    int current_value = tab[i-1][k].res+max_element(cut_value[i], cut_value[i]+4);
                    if(current_value > best_value)
                    {
                        best_value = current_value;
                        best_position = k;
                    }
                }
            }
            if(best_value == -1 || tab[i-1][j].res > best_value)
                tab[i][j] = tab[i-1][j];
            else{
                tab[i][j].cutted.push_back(i);
                int direction = distance(cut_value[j], max_element(cut_value[j], cut_value[j]+4));
                tab[i][j].directions.push_back()
            }
            //if(trees[i].d+calculate_distance(i, ))
        }
    }
    if(
            (treeCount <= 100 && float( clock () - begin_time ) /  CLOCKS_PER_SEC > TIME1) ||
            (treeCount <= 1000 && float( clock () - begin_time ) /  CLOCKS_PER_SEC > TIME2) ||
            (treeCount <= 10000 && float( clock () - begin_time ) /  CLOCKS_PER_SEC > TIME3)
            ) {
        print_result();
        return 0;
    }
    return 0;
}