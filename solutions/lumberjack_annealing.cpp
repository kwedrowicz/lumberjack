#include <iostream>
#include <ctime>
#include <vector>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include "../Forest.h"
#include "../Tree.h"
#include "../Solution.h"

#define MAXK 10000
#define MAXN 1000
#define TRIES 50
#define MAXPOWER 100
#define TIME1 0.95
#define TIME2 9.0
#define TIME3 58.0

using namespace std;

Forest * forest;
vector< Tree* > trees;
int cut_value[MAXK+1][4];

unsigned int t, n, k;

Solution * bestSolution;
Solution * currentSolution;
Solution * lastSolution;

double alfa = 0.5;
double beta = 0.5;

double delta = 0.9;
double c0 = 1.0;

string directions[] = {"down", "right", "up", "left"};

void calculate_cut_value() {
    for (int i = 1; i <= k; i++) {
        for (int j = 0; j < 4; j++) {
            int sum = trees[i]->getValue();
            int height = trees[i]->height;
            int x = trees[i]->x;
            int y = trees[i]->y;
            int dx = 0;
            int dy = 0;
            Tree * currentTree = trees[i];
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
                if(forest->getTree(x,y)){
                    if(forest->getTree(x,y)->isHeavier(*currentTree)){
                        currentTree = forest->getTree(x,y);
                        sum += currentTree->getValue();
                        height = currentTree->height;
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

int calculate_cut_tree_value_sum(int i, int direction) {
    trees[i]->isCut = true;
    int sum = trees[i]->getValue();
    int height = trees[i]->height;
    int x = trees[i]->x;
    int y = trees[i]->y;
    int dx = 0;
    int dy = 0;
    Tree * currentTree = trees[i];
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
        if(forest->getTree(x,y) && !trees[forest->getTree(x,y)->number]->isCut){
            if(forest->getTree(x,y)->isHeavier(*currentTree)){
                currentTree = forest->getTree(x,y);
                sum += currentTree->getValue();
                height = currentTree->height;
                trees[forest->getTree(x,y)->number]->isCut = true;
            }
            else
                break;
        }
        height--;
    }
    return sum;
}

double calculate_probability(int i, int j)
{
    double up =  pow((1.0 / (trees[i]->getDistance(*trees[j])+trees[j]->width)), alfa) * pow(*max_element(cut_value[j], cut_value[j]+4), beta);
    double down = 0;
    for(int l = 1; l <= k; l++)
    {
        if(!trees[l]->isCut){
            down += pow((1.0 / (trees[i]->getDistance(*trees[l])+trees[l]->width)), alfa) * pow(*max_element(cut_value[l], cut_value[l]+4), beta);
        }
    }
    return up / down;
}

void print_best(clock_t begin_time)
{
    cout <<"BEST: "<<bestSolution->value<<endl;
    for(int i = 0; i < bestSolution->path.size(); i++)
    {
        cout << bestSolution->path[i];
        if(bestSolution->directions[i] >= 0 && bestSolution->directions[i] <= 3)
            cout <<"("<< directions[bestSolution->directions[i]] <<")";
        if(i < bestSolution->path.size()-1)
            cout <<" -> ";
    }
    cout <<"\n";
    cout << float( clock () - begin_time ) /  CLOCKS_PER_SEC <<"\n";
    exit(0);
}

void print_result(){
    for(int i = 1; i < bestSolution->path.size(); i++)
    {
        int horizontal = trees[bestSolution->path[i]]->x-trees[bestSolution->path[i-1]]->x;
        int vertical = trees[bestSolution->path[i]]->y-trees[bestSolution->path[i-1]]->y;
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
        cout << "cut "<<directions[bestSolution->directions[i]]<<"\n";
    }
}

bool choose_tree()
{
    vector<double> p;
    p.resize(k+1);
    double total = 0.0;
    int last_tree_index = currentSolution->path.back();
    for(int j = 1; j <= k; j++)
    {
        if(!trees[j]->isCut && currentSolution->time >= trees[last_tree_index]->getDistance(*trees[j]) + trees[j]->width )
        {
            p[j] = calculate_probability(last_tree_index, j);
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
    for(int j = 1; j <= k; j++)
    {
        sum += p[j];
        if(r < sum)
        {
            currentSolution->path.push_back(j);
            int direction = (int)distance(cut_value[j], max_element(cut_value[j], cut_value[j]+4));
            currentSolution->directions.push_back(direction);
            int cost = trees[last_tree_index]->getDistance(*trees[j]) + trees[j]->width;
            currentSolution->time -= cost;
            currentSolution->value += calculate_cut_tree_value_sum(j,direction);
            //cout << "CUTTED "<<j<<endl;
            break;
        }
    }
    return true;
}

void generateSolution(){
    currentSolution = new Solution(t);
    while(currentSolution->time > 0){
        if(!choose_tree())
            break;
    }
}

void print_path(){
    for(int i = 0; i < currentSolution->path.size(); i++){
        cout << currentSolution->path[i];
        if(i < currentSolution->path.size()-1)
            cout << " -> ";
    }
    cout <<endl;
}

void recalculateSolution(){
    for(int i = 1; i <= k; i++)
        trees[i]->isCut = false;
    currentSolution->value = 0;
    currentSolution->time = t;
    for(int i = 1; i < currentSolution->path.size(); i++)
    {
        if(trees[currentSolution->path[i]]->isCut){
            currentSolution->path.erase(currentSolution->path.begin()+i);
            currentSolution->directions.erase(currentSolution->directions.begin()+i);
            i--;
            continue;
        }
        currentSolution->time -= (trees[currentSolution->path[i]]->getDistance(*trees[currentSolution->path[i-1]])+trees[currentSolution->path[i]]->width);
        currentSolution->value += calculate_cut_tree_value_sum(currentSolution->path[i], currentSolution->directions[i]);
    }

}

void localChange(){
    //cout << "LOCAL START"<<endl;
    unsigned long size = currentSolution->path.size();
    long r = rand() % (size-1) + 1;
    int additional_time = trees[currentSolution->path[r-1]]->getDistance(*trees[currentSolution->path[r]]);
    if(r < size-1)
        additional_time += trees[currentSolution->path[r]]->getDistance(*trees[currentSolution->path[r+1]]);
    additional_time += trees[currentSolution->path[r]]->width;
    currentSolution->time += additional_time;
    vector<int> possible_choose;
    for(int i = 1; i <= k; i++){
        int consume_time = trees[i]->width;
        consume_time += trees[i]->getDistance(*trees[currentSolution->path[r-1]]);
        if(r < size-1)
            consume_time += trees[i]->getDistance(*trees[currentSolution->path[r+1]]);
        if(consume_time <= currentSolution->time){
            possible_choose.push_back(i);
        }
    }
    int chooose_rand = int(rand() % possible_choose.size());
    currentSolution->path[r] = possible_choose[chooose_rand];
    //cout << "LOCAL END"<<endl;
    int direction = (int)distance(cut_value[possible_choose[chooose_rand]], max_element(cut_value[possible_choose[chooose_rand]], cut_value[possible_choose[chooose_rand]]+4));
    currentSolution->directions[r] = direction;
    recalculateSolution();
}

int main()
{
    const clock_t begin_time = clock();
    cin >> t >> n >> k;
    forest = new Forest(n);
    Tree * nullTree = new Tree(0,0,0,0,0,0);
    trees.push_back(nullTree);
    for(int i = 1; i <= k; i++)
    {
        int x, y, h, d, c, p;
        cin >> x >> y >> h >> d >> c >> p;
        Tree * tree = new Tree(h,d,c,p,x,y,i);
        forest->plantTree(x,y, tree);
        trees.push_back(tree);
    }
    srand( time( NULL ) );
    calculate_cut_value();
    while(true){
        generateSolution();
        for(int a = 1; a < MAXPOWER; a++){
            localChange();
            if(!lastSolution || currentSolution->value > lastSolution->value)
                lastSolution = currentSolution;
            else{
                double ck = pow(delta, a)*c0;
                if((lastSolution->value-currentSolution->value)*100/ck < (rand() % 100)){
                    lastSolution = currentSolution;
                }
            }
            if(
                    (k <= 100 && float( clock () - begin_time ) /  CLOCKS_PER_SEC > TIME1) ||
                    (k <= 1000 && float( clock () - begin_time ) /  CLOCKS_PER_SEC > TIME2) ||
                    (k <= 10000 && float( clock () - begin_time ) /  CLOCKS_PER_SEC > TIME3)
                    ){
                print_result();
                return 0;
            }
            if(!bestSolution || currentSolution->value > bestSolution->value){
                bestSolution = currentSolution;
            }
        }

    }
    print_result();
    return 0;
}

