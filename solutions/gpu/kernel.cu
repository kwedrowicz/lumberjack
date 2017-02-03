#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <cuda.h>
#include <device_functions.h>
#include <cuda_runtime_api.h>

#include <curand.h>
#include <curand_kernel.h>

#include <stdio.h>
#include <iostream>
#include <ctime>
 
#include "KernelAPI.h"
//cudaError_t addWithCuda(int *c, const int *a, const int *b, unsigned int size);

__global__ void initRand(curandState_t *states, unsigned int seed)
{
	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	curand_init(seed, tid, 0, &states[tid]);
}

__device__ int rouletteSelectTree(curandState_t *states, float *probs, int length)
{
	int idx = blockIdx.x * blockDim.x + threadIdx.x;
	float sum = 0;
	
	for (int i = 0; i < length; i++)
	{
		sum += probs[i];
	}
	
	if (sum < 0.001)
	{
	//	printf("hi ");
		return 0;
	}

	float rand = curand_uniform(&states[idx]) * sum * 0.999;
	//printf("%f ", rand);
/*	if (threadIdx.x == 0) {
		printf("SUM: %f RAND: %f \n", sum, rand);
	}*/
	float x = 0;
	for (int i = 0; i < length; i++)
	{
		x += probs[i];
/*		if (threadIdx.x == 0)
		{
			printf("PROB: %f PROBSUM: %f \n", probs[i], x);
		}*/
		if (x > rand)
		{
			return i;
		}
	}
	printf("REACHED UNREACHABLE SUM: %f  RAND: %f  X: %f\n", sum, rand, x);
	// shouldn't reach this place
	return 0;
}

__device__ float calcProbability(int *dists, float  *pheroms, int treeIndex, int *cutValues, int diameter)
{
	return powf(pheroms[treeIndex], ALPHA) * powf(1.0f / (dists[treeIndex] + diameter), BETA) * powf(cutValues[treeIndex * 2], GAMMA);
}

__device__ int cutTree(Tree *trees, bool *treeState, int i, int direction, int mapSize, int *forest)
{
	int height = trees[i].h;
	int diameter = trees[i].d;
	int volume = height * diameter;
	int weight = volume * trees[i].c;
	int sum = volume * trees[i].p;
	int x = trees[i].x;
	int y = trees[i].y;
	int cur_tree = i;
	treeState[cur_tree] = false;
	int dx, dy;
	if (direction == 0)
	{
		dx = 0;
		dy = -1;
	}
	if (direction == 1)
	{
		dx = 1;
		dy = 0;
	}
	if (direction == 2)
	{
		dx = 0;
		dy = 1;
	}
	if (direction == 3)
	{
		dx = -1;
		dy = 0;
	}
	height--;
	while (height > 0)
	{
		x += dx;
		y += dy;
		if (x < 0 || y < 0 || x >= mapSize || y >= mapSize)
			break;
		int treeIndex = forest[y * mapSize + x];
		if (treeIndex > 0 && treeState[treeIndex])
		{
			Tree t = trees[treeIndex];
			int w = t.h * t.d * t.c;
			if (weight > w)
			{
				sum += t.h * t.d * t.p;
				height = trees[treeIndex].h;
				weight = w;
				cur_tree = treeIndex;
				treeState[cur_tree] = false;
			}
			else
				break;
		}
		height--;
	}
	return sum;
}

__global__ void runAnt(int treeCount, int timeLimit, int mapSize, Tree *trees, int *distances, float* pheromones, int *cutValues, curandState_t *randStates, int *gpuPaths, int *scores, int *lengths, int *forest)
{
	__shared__ bool treeState[MAX_TREES]; // 0 for cut, 1 for available
	__shared__ int currentTree;
	__shared__ int currentTime;
	__shared__ int currentScore;
	__shared__ int numberVisited;
	__shared__ bool theEnd;
	__shared__ float sharedTreeProb[THREADS_PER_ANT]; 
	__shared__ int sharedTreeIdx[THREADS_PER_ANT];
	
	// cache 
//	__shared__ int localDistances[MAX_TREES];
//	__shared__ float localPheromones[MAX_TREES];
	//__shared__ float localCutValues[MAX_TREES*2];
	
	
	int treesPerThread = (treeCount + THREADS_PER_ANT - 1) / THREADS_PER_ANT;
	const int firstTree = threadIdx.x * treesPerThread;
	
	if (firstTree >= treeCount)
	{
		sharedTreeProb[threadIdx.x] = 0;
		return;
	}
	if (firstTree + treesPerThread > treeCount)
	{
		treesPerThread = treeCount - firstTree;
	}
	// Each thread chooses tree from it's own set, then main thread chooses one from these
	float *localTreeProb = new float[treesPerThread];

	// init tree states
	for (int i = firstTree; i < firstTree + treesPerThread; i++)
	{
		treeState[i] = true;
	}
	
	if (threadIdx.x == 0)
	{
		treeState[0] = false;
		currentTree = 0;
		currentTime = 0;
		numberVisited = 0; 
		currentScore = 0;
		theEnd = false;
	}
	__syncthreads();
	//int count = 100; // for safety / debugging
	while (!theEnd)
	{
		// wait for data transfers to complete
/* 	 if (threadIdx.x == 0)
		{
			printf("\n");
		}*/
		//__syncthreads();
		float sum = 0;
		for (int i = 0; i < treesPerThread; i++)
		{
			int tree = i + firstTree;
			int diameter = trees[tree].d;
			if (distances[treeCount*currentTree + tree] + diameter + currentTime <= timeLimit && treeState[tree]) // have enough time to cut and is not yet cut
			{
				localTreeProb[i] = calcProbability(&distances[treeCount*currentTree], &pheromones[treeCount*currentTree], tree, cutValues, diameter);
				sum += localTreeProb[i];
			} else
			{
				localTreeProb[i] = 0.0f;
			}
			//printf("%.2f ", localTreeProb[i]);
			
		}
	/*	if (treesPerThread > 0)
			printf("%.2f ", sum);*/
		
		int localSelectedTree = rouletteSelectTree(randStates, localTreeProb, treesPerThread);
		
		sharedTreeProb[threadIdx.x] = localTreeProb[localSelectedTree];
		sharedTreeIdx[threadIdx.x] = firstTree + localSelectedTree;
		__syncthreads();

		if (threadIdx.x == 0)
		{
			int nextIndex = rouletteSelectTree(randStates, sharedTreeProb, THREADS_PER_ANT);
			if (sharedTreeIdx[nextIndex] == 0)
			{
				// tour finished (end of game time)
	//			printf("Finishing ");
				theEnd = true;
				//break;
			}
			else {
				currentTime += distances[treeCount*currentTree + sharedTreeIdx[nextIndex]] + trees[sharedTreeIdx[nextIndex]].d;
				currentTree = sharedTreeIdx[nextIndex];

				gpuPaths[blockIdx.x * treeCount + numberVisited * 2] = currentTree;
				
				int dir = cutValues[currentTree * 2 + 1];

				gpuPaths[blockIdx.x * treeCount + numberVisited * 2 + 1] = dir;
				currentScore += cutTree(trees, treeState, currentTree, dir, mapSize, forest);
				numberVisited++;
			}
		}

		__syncthreads();
	}
	if (threadIdx.x == 0) {
		scores[blockIdx.x] = currentScore;
		lengths[blockIdx.x] = numberVisited;
	}
	delete[] localTreeProb;
}

curandState_t *randStates;
Tree *gpuTrees;
int *gpuDistances;
float *gpuPheromones;
int *gpuCutValues;
int *gpuPaths;
int *gpuScores;
int *gpuLengths;
int *gpuForest;
const int threadsPerBlock = THREADS_PER_ANT;
const int blocks = ANTS;


void initGPU(int numberOfTrees, int mapSize, Tree *trees, int *distances, int *cutValues, int *forest)
{
	cudaMalloc((void**)&randStates, sizeof(curandState_t) * threadsPerBlock * blocks);

	initRand << <blocks, threadsPerBlock >> >(randStates, time(0));

	cudaMalloc((void**)&gpuTrees, sizeof(Tree) * numberOfTrees);
	cudaMemcpy(gpuTrees, trees, sizeof(Tree)*numberOfTrees, cudaMemcpyHostToDevice);

	cudaMalloc((void**)&gpuDistances, sizeof(int) * numberOfTrees*numberOfTrees);
	cudaMemcpy(gpuDistances, distances, sizeof(int)*numberOfTrees*numberOfTrees, cudaMemcpyHostToDevice);

	cudaMalloc((void**)&gpuPheromones, sizeof(float)*numberOfTrees*numberOfTrees);

	cudaMalloc((void**)&gpuCutValues, sizeof(int)*numberOfTrees*2);
	cudaMemcpy(gpuCutValues, cutValues, sizeof(int)*numberOfTrees * 2, cudaMemcpyHostToDevice);

	cudaMalloc((void**)&gpuPaths, sizeof(int) * ANTS * numberOfTrees); // STORES INDICES AND DIRECTIONS; SIZE IS ENOUGH FOR HALF TREES-1

	cudaMalloc((void**)&gpuScores, sizeof(int) * ANTS);
	cudaMalloc((void**)&gpuLengths, sizeof(int) * ANTS);

	cudaMalloc((void**)&gpuForest, sizeof(int)*mapSize*mapSize);
	cudaMemcpy(gpuForest, forest, sizeof(int)*mapSize*mapSize, cudaMemcpyHostToDevice);

}

void cleanupGPU()
{
	cudaFree(randStates);
	cudaFree(gpuTrees);
	cudaFree(gpuDistances);
	cudaFree(gpuPheromones);
	cudaFree(gpuCutValues);
	cudaFree(gpuPaths);
	cudaFree(gpuScores);
	cudaFree(gpuForest);
	cudaFree(gpuLengths);
}

int runKernel(int numberOfTrees, float *pheromones, int timeLimit, int mapSize, int *resultScores, int *resultPaths, int *resultLengths)
{	
	cudaMemcpy(gpuPheromones, pheromones, sizeof(float)*numberOfTrees*numberOfTrees, cudaMemcpyHostToDevice);

	
	runAnt << < blocks, threadsPerBlock >> >(numberOfTrees, timeLimit, mapSize, gpuTrees, gpuDistances, gpuPheromones, gpuCutValues, randStates, gpuPaths, gpuScores, gpuLengths, gpuForest);


	cudaMemcpy(resultScores, gpuScores, sizeof(int)*ANTS, cudaMemcpyDeviceToHost);
	cudaMemcpy(resultLengths, gpuLengths, sizeof(int)*ANTS, cudaMemcpyDeviceToHost);
	cudaMemcpy(resultPaths, gpuPaths, sizeof(int)*ANTS*numberOfTrees, cudaMemcpyDeviceToHost);

	return 0;
}

/*   const int arraySize = 5;
const int a[arraySize] = { 1, 2, 3, 4, 5 };
const int b[arraySize] = { 10, 20, 30, 40, 50 };
int c[arraySize] = { 0 };

// Add vectors in parallel.
cudaError_t cudaStatus = addWithCuda(c, a, b, arraySize);
if (cudaStatus != cudaSuccess) {
fprintf(stderr, "addWithCuda failed!");
return 1;
}

// cudaDeviceReset must be called before exiting in order for profiling and
// tracing tools such as Nsight and Visual Profiler to show complete traces.
cudaStatus = cudaDeviceReset();
if (cudaStatus != cudaSuccess) {
fprintf(stderr, "cudaDeviceReset failed!");
return 1;
} */

// Helper function for using CUDA to add vectors in parallel. 
/*
cudaError_t addWithCuda(int *c, const int *a, const int *b, unsigned int size)
{
    int *dev_a = 0;
    int *dev_b = 0;
    int *dev_c = 0;
    cudaError_t cudaStatus;

    // Choose which GPU to run on, change this on a multi-GPU system.
    cudaStatus = cudaSetDevice(0);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaSetDevice failed!  Do you have a CUDA-capable GPU installed?");
        goto Error;
    }

    // Allocate GPU buffers for three vectors (two input, one output)    .
    cudaStatus = cudaMalloc((void**)&dev_c, size * sizeof(int));
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMalloc failed!");
        goto Error;
    }

    cudaStatus = cudaMalloc((void**)&dev_a, size * sizeof(int));
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMalloc failed!");
        goto Error;
    }

    cudaStatus = cudaMalloc((void**)&dev_b, size * sizeof(int));
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMalloc failed!");
        goto Error;
    }

    // Copy input vectors from host memory to GPU buffers.
    cudaStatus = cudaMemcpy(dev_a, a, size * sizeof(int), cudaMemcpyHostToDevice);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMemcpy failed!");
        goto Error;
    }

    cudaStatus = cudaMemcpy(dev_b, b, size * sizeof(int), cudaMemcpyHostToDevice);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMemcpy failed!");
        goto Error;
    }

    // Launch a kernel on the GPU with one thread for each element.
    addKernel<<<1, size>>>(dev_c, dev_a, dev_b);

    // Check for any errors launching the kernel
    cudaStatus = cudaGetLastError();
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "addKernel launch failed: %s\n", cudaGetErrorString(cudaStatus));
        goto Error;
    }
    
    // cudaDeviceSynchronize waits for the kernel to finish, and returns
    // any errors encountered during the launch.
    cudaStatus = cudaDeviceSynchronize();
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
        goto Error;
    }

    // Copy output vector from GPU buffer to host memory.
    cudaStatus = cudaMemcpy(c, dev_c, size * sizeof(int), cudaMemcpyDeviceToHost);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMemcpy failed!");
        goto Error;
    }

Error:
    cudaFree(dev_c);
    cudaFree(dev_a);
    cudaFree(dev_b);
    
    return cudaStatus;
} */
