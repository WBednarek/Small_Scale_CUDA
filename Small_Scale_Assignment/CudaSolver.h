#pragma once
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "ReadMatrixCSR.h"
#include "ReadMatrixELL.h"
#include <iostream>





void CUDASolver(ReadMatrixCSR &mat, std::vector<int> & X, int blockSize, int maxBlocks, double & timeToComplete);
void CUDASolver(ReadMatrixELL &mat, std::vector<int> & X, int blockSize, int maxBlocks, double & timeToComplete);
