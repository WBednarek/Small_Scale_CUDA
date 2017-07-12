#pragma once
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "ReadMatrixCSR.h"
#include "ReadMatrixELL.h"
#include <iostream>





void CUDASolver(ReadMatrixCSR &mat, std::vector<double>& X, std::vector<double>& Y, int blockSize, int maxBlocks, double & timeToComplete);
void CUDASolver(ReadMatrixELL &mat, std::vector<double>& X, std::vector<double>& Y, int blockSize, int maxBlocks, double & timeToComplete);
