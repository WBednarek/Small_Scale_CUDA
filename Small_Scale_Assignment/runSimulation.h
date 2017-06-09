#pragma once
#include "ReadMatrixCSR.h"
#include "CudaSolver.h"
#include <vector>
#include <chrono>
#include <random>
class runSimulation
{
	std::vector<int> X;

public:
	runSimulation();
	~runSimulation();
	auto calcuatePerformance(ReadMatrixCSR & matrix, double completionTime);
	void runCSRCUDA(ReadMatrixCSR &mat, int numberOfThreads, int sizeOfBlock, int maximumBlocksdouble, double & timeToComplete);

	

	void makeVector(int sizeOfVector);


};

