#pragma once
#include "Structs.h"

void initGPU(int, int, Tree*, int*, int*, int*);
int runKernel(int, float*, int, int, int*, int*, int*);
void cleanupGPU();
