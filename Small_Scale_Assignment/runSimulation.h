#pragma once
#ifndef _RUNSIMULATION_H_
#define _RUNSIMULATION_H_

#include "ReadMatrixCSR.h"
#include "CudaSolver.h"
#include <vector>
#include <chrono>
#include <random>
#include <type_traits>

template<class classType>
class runSimulation
{
	std::vector<int> X;

public:
	runSimulation();
	~runSimulation();
	
	auto calcuatePerformance(classType & matrix, double completionTime);
	/**
	Runs CUDA solution.
	*/
	void runCUDA(classType &mat, int numberOfThreads, int sizeOfBlock, int maximumBlocksdouble, double & timeToComplete);

	void makeVector(int sizeOfVector);
	/**
	Creates an empty object to resolve template linker problem
	*/
	


};



#endif
