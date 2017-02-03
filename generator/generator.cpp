#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <vector>
#include <cassert>
#include <string>
#include "Forest.h"
#include "ForestGenerator.h"
#include "Constants.h"
#define MAX_SIZE 1000
#define MAX_VAL 100

using namespace std;

// 8 25x25
// 10 100x100
// 10 1000x1000

int main(int argc, char* argv[]){
  srand( (unsigned int)time( NULL ) );
  if(argc != 2){
    cout <<"Nie podałeś wielkości lasu\n";
  }
  int n = atoi(argv[1]);
  ForestGenerator fGen;
  /*
  Constants::limitBySize(n);
  Forest *testForest = fGen.generateForest(n, 1, false, false);
  testForest->printTrees();
  delete testForest;
  return 0;
  */
  //forest->calcTime(divisor);
  //forest->saveForest("test001.in");
  
  

  // *********** Generate test files *************************
  // tests 1, 2 were written by hand
  int start = 19; // 
  int end = 20; //
  Forest *forest = nullptr; // fGen.generateForest(10, 1, false, false); // this should never be written to a file
  for (int i = start; i < end; i++)
  {
	  string filename = "lumberjack";
	  if (i + 1 < 10)
	  {
		  filename += '0';
	  }
	  filename += to_string(i + 1) + ".in";
	  int size = 0;
	  Constants::restore();
	  // generateForest(size, number of crossings, generate noise trees (currently set at 5% space), randomize lines)
	  // 25x25 times 8
	  if (i < 10)
	  {
			size = 50;
			Constants::limitBySize(size);
			Constants::limitNoTrees(100);
			switch (i)
			{
			case 0:
				forest = fGen.generateForest(size, 10, false, false, 25);
				break;
			case 1:		  
				forest = fGen.generateForest(size, 10, true, false, 25);
				break;
			case 2:
				forest = fGen.generateForest(size, 10, false, true, 25);
				break;
			case 3:
				forest = fGen.generateForest(size, 10, true, true, 25);
				break;
			case 4:
				forest = fGen.generateForest(size, 5, false, false, 25);
				break;
			case 5:
				forest = fGen.generateForest(size, 5, true, false, 25);
				break;
			case 6:
				forest = fGen.generateForest(size, 5, false, true, 25);
				break;
			case 7:
				forest = fGen.generateForest(size, 5, true, true, 25);
				break;
			case 8:
				forest = fGen.generateForest(size, 3, true, true, 25);
				break;
			case 9:
				forest = fGen.generateForest(size, 15, false, false, 10);
			}

			forest->calcTime(10);
	  }
	  // 100x100 times 10
	  else if (i < 20)
	  {
		  size = 250;
		  Constants::restore();
		  Constants::limitNoTrees(950);
		  switch (i)
		  {
		  case 10:
			  forest = fGen.generateForest(size, 100, false, true, 2);
			  break;
		  case 11:
			  forest = fGen.generateForest(size, 100, true, false, 2);
			  break;
		  case 12:
			  forest = fGen.generateForest(size, 200, true, false, 2);
			  break;
		  case 13:
			  forest = fGen.generateForest(size, 500, false, false, 2);
			  break;
		  case 14:
			  Constants::limitNoTrees(1500);
			  forest = fGen.generateForest(size, 500, false, false, 2);
			  break;
		  case 15:
			  Constants::limitNoTrees(1500);
			  forest = fGen.generateForest(size, 500, false, true, 2);
			  break;
		  case 16:
			  Constants::limitNoTrees(800);
			  forest = fGen.generateForest(size, 300, false, false, 2);
			  break;
		  case 17:
			  Constants::limitNoTrees(800);
			  forest = fGen.generateForest(size, 200, false, false, 2);
			  break;
		  case 18:
			  forest = fGen.generateForest(size, 100, false, false, 2);
			  break;
		  case 19:
			  // max_value 10
			  forest = fGen.generateForest(size, 150, false, true, 2);
			  break;
		  }
		  forest->calcTime(20);
	  }
	  // 1000x1000 times 10
	  else
	  {
		  Constants::restore();
		  size = 1000;
		  Constants::limitNoTrees(10000);
		  switch (i)
		  {
		  case 29:		  
			  forest = fGen.generateForest(size, 1500, false, true, 1);
			  break;
		  case 28:
			  forest = fGen.generateForest(size, 2000, true, false, 1);
			  break;
		  case 20:
			//  Constants::limitNoTrees(size*size * 20 / 100);
			  forest = fGen.generateForest(size, 2000, false, true, 1);
			  break;
		  case 21:
			  forest = fGen.generateForest(size, 2000, true, true, 1);
			  break;
		  case 22:
			//  Constants::limitNoTrees(size*size * 15 / 100);
			  forest = fGen.generateForest(size, 2500, false, false, 1);
			  break;
		  case 23:
			  forest = fGen.generateForest(size, 2500, true, false, 1);
			  break;
		  case 24:
			  forest = fGen.generateForest(size, 2500, false, true, 1);
			  break;
		  case 25:
			  forest = fGen.generateForest(size, 2500, true, true, 1);
			  break;
		  case 26:
			  forest = fGen.generateForest(size, 4000, true, true, 1);
			  break;
		  case 27:
			  forest = fGen.generateForest(size, 750, true, true, 1);
			  break;
		  }
		  forest->calcTime(200);
	  }
	  
	  //Constants::limitNoTrees(size*size * 20 / 100);
	  if (forest->getNoTrees() < Constants::MAX_NO_TREES / 3)
	  {
		  i--;
		  delete forest;
		  continue;
	  }
	  forest->saveForest(filename);
	  delete forest;
	  //cout << filename << endl;
  }


  return 0;
}
