#pragma once
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "ReadMatrixCSR.h"
#include <iostream>





void CSRCUDASolver(ReadMatrixCSR &mat, std::vector<int> & X, int blockSize, int maxBlocks, double & timeToComplete);

