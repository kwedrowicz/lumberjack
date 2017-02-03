#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>

#define MAXK 10000
#define MAXN 1000
#define ANTS 50
#define ITERATIONS 50

using namespace std;

struct tree{
    int x, y, h, d, c, p;
};

struct ant{
    int i, t, res, total;
    vector<int> cutted;
    vector<int> directions;
};

string directions[] = {"down", "right", "up", "left"};

int t, n, k;

double alfa = 0.5;
double beta = 0.5;
double gam = 0.5;
double p = 0.5;
int iterations = ITERATIONS;
int best = 0;

vector<tree> trees;
double matrix[MAXK+1][MAXK+1];
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

int calculate_cut_tree_value_sum(int i, int direction)
{
    int sum = tree_value(i);
    int height = trees[i].h;
    int x = trees[i].x;
    int y = trees[i].y;
    int cur_tree = i;
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
        if(x < 0 || y < 0 || x >= n || y >= n)
            break;
        if(forest[x][y] > 0)
        {
            if(tree_weight(cur_tree) > tree_weight(forest[x][y]))
            {
                sum += tree_value(forest[x][y]);
                height = trees[forest[x][y]].h;
                cur_tree = forest[x][y];
            }
            else
                break;
        }
        height--;
    }
    return sum;
}
void calculate_cut_value()
{
    for(int i = 1; i <= k; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            int sum = tree_value(i);
            int height = trees[i].h;
            int x = trees[i].x;
            int y = trees[i].y;
            int cur_tree = i;
            int dx, dy;
            if(j == 0)
            {
                dx = 0;
                dy = -1;
            }
            if(j == 1)
            {
                dx = 1;
                dy = 0;
            }
            if(j == 2)
            {
                dx = 0;
                dy = 1;
            }
            if(j == 3)
            {
                dx = -1;
                dy = 0;
            }
            height--;
            while(height > 0)
            {
                x += dx;
                y += dy;
                if(x < 0 || y < 0 || x >= n || y >= n)
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
            cout << sum <<" ";
        }
        cout <<endl;
    }
}


double calculate_probability(int i, int j)
{
    int mi, mj;
    if(i < j)
    {
        mi = i;
        mj = j;
    }
    else
    {
        mi = j;
        mj = i;
    }
    double up = pow(matrix[mj][mi], alfa) * pow((1.0 / (matrix[mi][mj]+trees[j].d)), beta) * pow(*max_element(cut_value[j], cut_value[j]+4), gam);
    double down = 0;
    for(int l = 1; l <= k; l++)
    {
        if(!cutted[l]){
            down += pow(matrix[l][mi], alfa) * pow((1.0 / (matrix[mi][l]+trees[j].d)), beta) * pow(*max_element(cut_value[j], cut_value[j]+4), gam);
        }
    }
    cout << up <<"/"<<down<<endl;
    return up / down;
}

void evaporation(int i, int j)
{
    double f = (1-p)*matrix[i][j];
    for(int ant = 0; ant < ANTS; ant++)
    {
        for(int a = 1; a < ants[ant]->cutted.size(); a++)
        {
            if((ants[ant]->cutted[a-1] == j && ants[ant]->cutted[a] == i) || (ants[ant]->cutted[a-1] == i && ants[ant]->cutted[a] == j))
            {
                    f += (1 / double(ants[ant]->total));
                    f += (ants[ant]->res/(double)best);
            }
        }
    }
    matrix[i][j] = f;
}

bool choose_tree(int ant_num)
{
    cout <<"ANT IN "<<ants[ant_num]->i<<endl;
    vector<double> p;
    p.resize(k+1);
    double total = 0.0;
    for(int j = 1; j <= k; j++)
    {
        cout << cutted[j] << " "<<ants[ant_num]->t<<" "<<calculate_distance(ants[ant_num]->i,j) + trees[j].d<< endl;
        if(ants[ant_num]->i != j && !cutted[j] && ants[ant_num]->t >= (calculate_distance(ants[ant_num]->i,j) + trees[j].d) )
        {
            p[j] = calculate_probability(ants[ant_num]->i, j);
            total += p[j];
        }
        else
        {
            p[j] = 0;
        }
        cout <<"P["<<j<<"] = "<<p[j]<<endl;
    }
    if(total == 0.0)
        return false;
    double r = rand() % 100 / 100.0;
    r *= total;
    double sum = 0.0;
    //cout << r<< " "<<total<<"\n";
    for(int j = 1; j <= k; j++)
    {
        sum += p[j];
        if(r < sum)
        {
            ants[ant_num]->cutted.push_back(j);
            int direction = distance(cut_value[j], max_element(cut_value[j], cut_value[j]+4));
            ants[ant_num]->directions.push_back(direction);
            int cost = (matrix[ants[ant_num]->i][j] + trees[j].d);
            int value = tree_value(j);
            ants[ant_num]->t -= cost;
            ants[ant_num]->res += calculate_cut_tree_value_sum(j,direction);
            ants[ant_num]->i = j;
            ants[ant_num]->total += cost;
            cutted[j] = true;
            //cout << "CUTTED "<<j<<endl;
            break;
        }
    }
    return true;
}

void print_best(clock_t begin_time)
{
    cout <<"BEST: "<<best<<endl;
    for(int i = 0; i < best_ant.cutted.size(); i++)
    {
        cout << best_ant.cutted[i];
        if(best_ant.directions[i] >= 0 && best_ant.directions[i] <= 3)
         cout <<"("<< directions[best_ant.directions[i]] <<")";
        if(i < best_ant.cutted.size()-1)
            cout <<" -> ";
    }
    cout <<"\n";
    cout << float( clock () - begin_time ) /  CLOCKS_PER_SEC <<"\n";
}

int main()
{
    const clock_t begin_time = clock();
    cin >> t >> n >> k;
    trees.resize(k+1);
    for(int i = 1; i <= k; i++)
    {
        int x, y, h, d, c, p;
        cin >> x >> y >> h >> d >> c >> p;
        forest[x][y] = i;
        trees[i] = {x,y,h,d,c,p};
    }
    /*cout <<"FOREST\n";
    for(int i = 0; i < n; i++)
    {
        for(int j = 0; j < n; j++)
        {
            cout << forest[i][j] <<" ";
        }
        cout <<"\n";
    }*/
    srand( time( NULL ) );
    trees[0] = {0,0,0,0,0,0};
    for(int i = 0; i < trees.size(); i++)
    {
        for(int j = 0; j < i; j++)
        {
            matrix[i][j] = 1;
        }
        for(int j = i; j < trees.size(); j++)
        {
            matrix[i][j] = calculate_distance(i,j);
        }
    }
    calculate_cut_value();
    while(iterations-- > 0)
    {
        /*cout << "ITERATION "<<ITERATIONS-iterations<<"\n";
        for(int i = 0; i < trees.size(); i++)
        {
            for(int j = 0; j < trees.size(); j++)
            {
                cout << matrix[i][j] <<" ";
            }
            cout <<"\n";
        }*/
        for(int i = 0; i < ANTS; i++)
        {
            //cout << "ANT NUMBER "<<i<<endl;
            ants[i] = new ant();
            ants[i]->i = 0;
            ants[i]->t = t;
            ants[i]->res = 0;
            ants[i]->total = 0;
            ants[i]->cutted.push_back(0);
            ants[i]->directions.push_back(-1);
            for(int a = 0; a <= k; a++)
            {
                cutted[a] = false;
            }
            cutted[0] = true;
            while(ants[i]->t > 0){
                if(!choose_tree(i))
                    break;
            }
            if(ants[i]->res > best)
            {
                //cout <<"BEST "<<best<<" -> "<<ants[i]->res<<endl;
                best = ants[i]->res;
                best_ant = *ants[i];
            }
            if(float( clock () - begin_time ) /  CLOCKS_PER_SEC > 0.95)
            {
                print_best(begin_time);
                return 0;
            }
        }

        for(int i = 0; i < trees.size(); i++)
        {
            for(int j = 0; j < i; j++)
            {
                evaporation(i,j);
            }
        }
    }
    print_best(begin_time);
    return 0;
}
