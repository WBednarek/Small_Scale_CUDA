#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "mmio.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <tuple>


class ReadMatrixCSR
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
	int * IRP; // Pointer to IRP array

	//std::vector<std::pair<int, double> > rowsAndValues;
	std::vector<std::tuple<int, int, double> > rowsAndValues;

	void calculateCSRValues();

	void displayPointerArray(int * arr);

	

public:	

	ReadMatrixCSR(std::string matrixName);
	~ReadMatrixCSR();
	std::vector<int> getIRP();
	std::vector<double> getAS();
	std::vector<int> getJA();
	int getM();
	int getNZ();
	int getN();

};

