#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "mmio.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>


class ReadMatrix
{

private:
	int ret_code;
	MM_typecode matcode;
	FILE *f;
	int M, N, nz;
	int i, *I, *J;
	double *matrixValue;
	std::string matrixName;
	int * JA;
	double * AS;
	int * IRP;

	std::vector<std::pair<int, double> > rowsAndValues;
	

	void calculateCSRValues();

	void displayPointerArray(int * arr);

	

public:	

	ReadMatrix(std::string matrixName);
	~ReadMatrix();
	std::vector<int> getIRP();
	std::vector<double> getAS();
	std::vector<int> getJA();

};

