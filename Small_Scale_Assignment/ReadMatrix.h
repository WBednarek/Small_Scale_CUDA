#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "mmio.h"
class ReadMatrix
{

private:
	int ret_code;
	MM_typecode matcode;
	FILE *f;
	int M, N, nz;
	int i, *I, *J;
	double *val;
	std::string matrixName;
public:	

	ReadMatrix(std::string matrixName);
	~ReadMatrix();

};

