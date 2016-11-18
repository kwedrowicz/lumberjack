#ifndef LUMBERJACK_SOLUTION_H
#define LUMBERJACK_SOLUTION_H

#include <vector>

using namespace std;

class Solution {
public:
    int value = 0;
    int time;
    vector<int> path;
    vector<int> directions;
    Solution(int t);
};


#endif //LUMBERJACK_SOLUTION_H
