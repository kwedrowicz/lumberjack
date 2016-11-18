#include <iostream>
#include "Forest.h"
#include "Tree.h"

Forest::Forest(int n){
    this->n = n;
    tab.resize(n);
    for(int i = 0; i < n; i++){
        tab[i].resize(n);
    }
}
Forest::~Forest() {
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++)
        {
            if(tab[i][j] != nullptr)
                delete tab[i][j];
        }
    }
}
bool Forest::isTree(int x, int y){
    return tab[x][y] != nullptr;
}
void Forest::removeTree(int x, int y) {
    delete tab[x][y];
    tab[x][y] = nullptr;
}
int Forest::getSize() {
    return tab.size();
}
// If plot is empty plant tree; returns plot value before planting
Tree* Forest::plantTree(int x, int y, Tree* tree) {
    if(tab[x][y] != nullptr)
        return tab[x][y];
    else {
        tab[x][y] = tree;
        return nullptr;
    }
}

Tree* Forest::getTree(int x, int y){
    if(tab[x][y] != nullptr)
        return tab[x][y];
    else
        return nullptr;
}

void Forest::seedRandomTree(){
    int x,y;
    do {
        x = rand()%n;
        y = rand()%n;
    } while(isTree(x,y));
    tab[x][y] = new Tree(rand()%20+1, rand()%10+1, rand()%10+1, rand()%10+1);
}

void Forest::printTrees(){
    cout << "Size: " << n << endl ;
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            if(isTree(i,j)){
                cout << i <<" "<< j <<" "<< tab[i][j]->height <<" "<< tab[i][j]->width <<" "<< tab[i][j]->weight_unit <<" "<< tab[i][j]->value_unit <<"\n";
            }
        }
    }
}