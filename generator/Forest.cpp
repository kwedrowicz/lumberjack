#include <iostream>
#include <fstream>
#include "Forest.h"
#include "Tree.h"
#include "Constants.h"
#include <algorithm>

  Forest::Forest(int n){
    this->n = n;
    tab.resize(n);
    for(int i = 0; i < n; i++){
      tab[i].resize(n);
    }
	treeCount = 0;
  }
  Forest::~Forest() {
  	for(int i = 0; i < n; i++) {
  		for(int j = 0; j < n; j++)
  		{
			if (tab[i][j] != nullptr)
			{
				delete tab[i][j];				
			}
  		}
  	}
  }
  bool Forest::isTree(int x, int y){
    return tab[x][y] != nullptr;
  }
  int Forest::getNoTrees()
  {
	  return treeCount;
  }
  Tree* Forest::getTree(int x, int y) {
	  return tab[x][y];
  }
  void Forest::removeTree(int x, int y) {
	delete tab[x][y];
	tab[x][y] = nullptr;
	treeCount--;
	
  }
  int Forest::getSize() {
  	return tab.size();
  }
  int Forest::calcTime(int multiplier) {
/*	  int widthSum = 0;
	  for (int i = 0; i < n; ++i)
	  {
		  for (int j = 0; j < n; ++j)
		  {
			  if ((i != 0 || j != 0) && isTree(i, j))
				  widthSum += tab[i][j]->width;
		  }
	  }
	  time = 2*n;
	  time += (widthSum / treeCount) * multiplier;
	  if (time > Constants::MAX_TIME)
		  time = Constants::MAX_TIME;*/
	  time = static_cast<int> (this->getSize() * 1.75);
	  return time;
  }
  // If plot is empty plant tree; returns plot value before planting
  Tree* Forest::plantTree(int x, int y, Tree* tree) {
  	if(tab[x][y] != nullptr)
  		return tab[x][y];
  	else {
		treeCount++;
  		tab[x][y] = tree;
		tab[x][y]->x = x;
		tab[x][y]->y = y;
  		return nullptr;
  	}
  }
  void Forest::seedRandomTree(){
      int x,y;
      do {
        x = rand()%n;
        y = rand()%n;
      } while(isTree(x,y));
	  plantTree(x, y, new Tree(rand() % Constants::MAX_HEIGHT + 1, rand() % Constants::MAX_WIDTH + 1, rand() % Constants::MAX_WEIGHT + 1, rand() % Constants::MAX_VALUE + 1));
  }

  Tree* Forest::getRandomTree()
  {
	  int x, y;
	  do {
		  x = rand() % n;
		  y = rand() % n;
	  } while (!isTree(x, y));
	  return tab[x][y];
  }

  void Forest::saveForest(string filename) {
	  ofstream file(filename, fstream::out | fstream::trunc);
	  // clear the starting area
	  for (int i = 0; i < 2; i++)
	  {
		  for (int j = 0; j < 2; j++)
		  {
			  if (i != j && isTree(i, j)) {
				  removeTree(i, j);
			  }
		  }
	  }


	  file << time << " " << n << " " << treeCount << endl;
	  for (int i = 0; i < n; ++i)
	  {
		  for (int j = 0; j < n; ++j)
		  {
			  if (isTree(i, j))
			  {
				  file << tab[i][j]->getFileString(i, j);
			  }
		  }
	  }
	  file.close();
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
