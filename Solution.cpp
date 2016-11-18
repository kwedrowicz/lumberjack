//
// Created by krzysztof on 31.10.16.
//

#include "Solution.h"

Solution::Solution(int t) {
    time = t;
    path.push_back(0);
    directions.push_back(-1);
}