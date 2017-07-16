#pragma once
#ifndef _RUNSIMULATION_H_
#define _RUNSIMULATION_H_

#include "ReadMatrixCSR.h"
#include "ReadMatrixELL.h"
#include "CudaSolver.h"
#include "OpenMP.h"
#include <vector>
#include <chrono>
#include <random>
#include <type_traits>

template<class classType>
class SimulationAndTheTests
{
	

public:
	SimulationAndTheTests();
	~SimulationAndTheTests();
	
	auto calcuatePerformance(classType & matrix, double completionTime);
	/**
	Runs CUDA solution.
	*/
	void runCUDA(classType &mat, int numberOfThreads, int sizeOfBlock, int maximumBlocksdouble, double & timeToComplete);

	void runOpenMP(classType &mat, int numberOfThreads, int numberOfMatrixXColumns, int numberOfSimulationRuns);

	/**
	Make an vector for parallel matrix-vector multiplication
	*/
	void makeVector_X(std::vector<double> & X, int sizeOfMaxtixRow, int numberOfMatrixXColumns);
	
	


};



#endif


