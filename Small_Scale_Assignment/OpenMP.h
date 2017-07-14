#pragma once
#include "ReadMatrixCSR.h"
#include "ReadMatrixELL.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <type_traits>
class OpenMP
{
public:
	OpenMP();
	~OpenMP();

	void OpenMPSolver(ReadMatrixCSR &mat, std::vector<double> & X, std::vector<double> & Y, unsigned int threadsNumber, double & timeToComplete);


	void OpenMPSolver(ReadMatrixELL &mat, std::vector<double> & X, std::vector<double> & Y, unsigned int threadsNumber, double & timeToComplete, unsigned int numberOfMatrixXColumn);
};

